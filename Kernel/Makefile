CC = gcc
OUTFILE = main.out

SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
OBJ = $(SRC:%.c=./obj/%.o)
SHARED = $(shell find ../Shared/obj/*.o)

build: makefolder makeshared $(OBJ)
	$(CC) -o $(OUTFILE) main.c $(OBJ) $(SHARED)


LINK = -lpthread -lcommons -lm
CFLAGS = -c

./obj/%.o: ./src/%.c
	$(CC) $(CFLAGS) $< -o $@ $(LINK)

makefolder:
	if [ ! -d ./obj ]; then mkdir obj; fi

makeshared:
	make -C ../Shared

clean: 
	rm -rf obj
	rm -rf *.out
	make clean -C ../Shared