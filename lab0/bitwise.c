#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BIT(n) (1<<(n))

char *byte2bin(uint8_t n, char *binstr)
{
    // one element per bit (0/1)
    uint8_t binary[8];

    int i = 0;
    for(i = 0; i < 8; i++) {
        binary[i] = n % 2;
        n = n / 2;
    }

    // printing binary array in reverse order
    for (int j = 7, k= 0; j >= 0; j--, k++)
        sprintf(binstr + k, "%d", binary[j]);

    return binstr;
}

void print_usage(char *name) {
    printf("Usage: %s <action> <byte> <bit no>\n", name);
    printf("\tWhere: <action> one of 'h'|'l'|'r'|'s'|'t'\n"
           "\t\t <byte> 8-bit integer in hexadecimal\n"
           "\t\t <bit no> of bit to operate on\n");
}

int main(int argc, char *argv[])
{
    char a;	// action: 'h', 'l', 'r', 's', 't'
    unsigned long n; // value to convert must be smaller than 256
    int bit; // bit to operate on: must be between 0 an 7
    char binary[9]; // array for binary representation of n

    // Validate command line arguments
    if (argc != 4) {
        printf("Error: Incorrect number of arguments.\n");
        print_usage("bitwise");
        return EXIT_FAILURE;
    }


    a = argv[1][0];
    if (a != 'h' && a != 'l' && a != 'r' && a != 's' && a != 't') {
        printf("Error: Incorrect option for the first argument.\n");
        printf("It must be: 'l', 'r', 's', 't'\n");
        return EXIT_FAILURE;
    }


    n = strtol(argv[2], NULL, 16);
    if (n >= 256) {
        printf("Error: Incorrect option for the second argument.\n");
        printf("The 2nd argument must be positive and smaller than 256 (100 in hexadecimal).\n");
        return EXIT_FAILURE;
    }


    // Print to stdout the binary representation of n
    byte2bin(n, binary);

    bit = atoi(argv[3]);

    if (bit < 0 || bit > 7) {
        printf("Error: Incorrect option for the third argument.\n");
        printf("The 3nd argument must be an integer between 0 and 7.\n");
        return EXIT_FAILURE;
    }


    // Print the arguments
    printf("--@ Chosen options @--\n");
    printf("1. Action to perform: %c\n", a);
    printf("2. Binary number: %s\n", binary);
    printf("3. Bit: %d\n", bit);
    printf("\n--@ Operation @--\n");

    // Do what the user asked and print the result
    unsigned long mask = BIT(bit);
    switch (a) {
        case 'h':
            if (n & mask) {
                printf("Bit is high\n");
            } else printf("Bit is not high\n");
            break;
        case 'l':
            if (~n & mask) {
                printf("Bit is low\n");
            } else printf("Bit is not low\n");
            break;
        case 'r':
            n &= ~mask;
            byte2bin(n, binary);
            printf("New binary number: %s\n", binary);
            break;
        case 's':
            n |= mask;
            byte2bin(n, binary);
            printf("New binary number: %s\n", binary);
            break;
        case 't':
            n ^= mask;
            byte2bin(n, binary);
            printf("New binary number: %s\n", binary);
            break;
        default:
            break;
    }
    return 0;
}