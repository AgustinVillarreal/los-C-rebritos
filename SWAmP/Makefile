CC = gcc
OUTFILE = swamp.out

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)
SHARED = $(shell find ../Shared/obj/*.o)

build: makeshared makefolder  $(OBJ)
	$(CC) -o $(OUTFILE) main.c $(OBJ) $(LINK)


LINK = -lpthread -lcommons -lm -lshared
CFLAGS = -c

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi

# puede dar error, verlo en el futuro
makeshared:
	make install -C ../Shared

debug: CFLAGS := $(CFLAGS) -g
debug: build

valgrind: debug build
	valgrind ./$(OUTFILE) --leak-check=full --track-origins=yes --log-file=valgrind.log

clean: 
	rm -rf obj
	rm -rf *.out
	make clean -C ../Shared
