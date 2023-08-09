#include <lcom/lcf.h>
#include <machine/int86.h>

#include <macro.h>
#include "math.h"
#include "video_gr.h"

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bpp;
static unsigned bits_per_pixel;
static unsigned int vram_base;
static unsigned int vram_size;
static char* video_mem;
static bool indexed_mode;
static uint8_t red_mask_size;
static uint8_t green_mask_size;
static uint8_t blue_mask_size;


void *(vg_init)(uint16_t mode) {
    vbe_mode_info_t vmi_p;
    if (vbe_get_mode_info(mode, &vmi_p)) {
        return NULL;
    }


    h_res = vmi_p.XResolution;
    v_res = vmi_p.YResolution;
    bits_per_pixel = vmi_p.BitsPerPixel;
    bpp = ceil(vmi_p.BitsPerPixel / 8.0);
    vram_base = vmi_p.PhysBasePtr;
    vram_size = h_res * v_res * bpp;

    red_mask_size = vmi_p.RedMaskSize;
    green_mask_size = vmi_p.GreenMaskSize;
    blue_mask_size = vmi_p.BlueMaskSize;

    struct minix_mem_range mr;
    int error;

    /* Allow memory mapping */

    mr.mr_base = (phys_bytes) vram_base;	
    mr.mr_limit = mr.mr_base + vram_size;  

    if( OK != (error = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
        panic("sys_privctl (ADD_MEM) failed: %d\n", error);
        return NULL;    
    }

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if(video_mem == MAP_FAILED) {
        panic("couldn't map video memory");
        return NULL;
    }

    
    reg86_t r;

    memset(&r, 0, sizeof(r));

    r.ax = SET_VBE_MODE;
    r.bx = BIT(14) | mode;
    r.intno = 0x10; 
    if (sys_int86(&r) != OK) { 
        printf("set_vbe_mode: sys_int86() failed \n");

        return NULL; 
    }
    
    indexed_mode = (mode == 0x105);

    return video_mem;
}

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color) {
    char* pos = (video_mem + (y * h_res + x) * bpp);
    for (unsigned int i = 0; i < bpp; i++) {
        if (memcpy(pos, &color, bpp) == NULL) return 1;
    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color)) return 1;
    }
    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
    for (uint16_t i = 0; i < height; i++) {
        if (vg_draw_hline(x, y + i, width, color)) return 1;
    }
    return 0;
}

uint8_t clear_bits(uint8_t color, uint8_t starting_pos){
    if (starting_pos == 8) return color;

    for (int i = (int)starting_pos; i < 8; i++){
        color &= (~BIT(i));
    }

    return color;
}

int (vg_draw_pattern)(uint8_t no_rectangles, uint32_t first, uint8_t step) {
    uint16_t width, height;
    width = h_res / no_rectangles;
    height = v_res / no_rectangles;

    uint8_t red = (uint8_t)(first >> (green_mask_size + blue_mask_size));
    red = clear_bits(red, red_mask_size);
    uint8_t green = (uint8_t)(first >> (blue_mask_size));
    green = clear_bits(green, green_mask_size);
    uint8_t blue = (uint8_t)first;
    blue = clear_bits(blue, blue_mask_size);

    uint32_t color;
    for (uint8_t row = 0; row < no_rectangles; row++) {
        for (uint8_t col = 0; col < no_rectangles; col++) {
            if (indexed_mode) {
                color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
            } else {
                uint8_t new_red, new_green, new_blue;

                new_red = (red + col * step) % (1 << red_mask_size);
                new_green = (green + row * step) % (1 << green_mask_size);
                new_blue = (blue + (col + row) * step) % (1 << blue_mask_size);

                color = (new_red << (green_mask_size + blue_mask_size)) | (new_green << blue_mask_size) | new_blue;
            }
            if (vg_draw_rectangle(col * width, row * height, width, height, color)) return 1;
        }
    }
    return 0;
}

int (vg_erase_xpm)(xpm_image_t img_info, uint16_t x, uint16_t y) {
    uint16_t width_pixels = img_info.width, height_pixels = img_info.height;

    for (uint8_t row = 0; row < height_pixels; row++) {
        for (uint8_t col = 0; col < width_pixels; col++) {
            vg_draw_pixel(x + col, y + row, xpm_transparency_color(img_info.type));
        }
    }

    return 0;
}

int (vg_draw_xpm)(xpm_image_t img_info, uint16_t x, uint16_t y) {
    uint16_t width_pixels = img_info.width, height_pixels = img_info.height;

    for (uint8_t row = 0; row < height_pixels; row++) {
        for (uint8_t col = 0; col < width_pixels; col++) {
            uint8_t current_color = *(img_info.bytes + (row * width_pixels) + col);
            if (current_color == xpm_transparency_color(img_info.type)) continue;
            vg_draw_pixel(x + col, y + row, current_color);
        }
    }

    return 0;
}
