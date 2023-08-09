FLAGS+= -Wall -Wuninitialized -Werror
FLAGS+= -lm
FLAGS+=-fsanitize=address -fsanitize=undefined

CPP_FILES=
HEADERS=
PROG=main

$(PROG): $(CPP_FILES) $(HEADERS)
	gcc $(FLAGS) $(CPP_FILES) -o $(PROG)