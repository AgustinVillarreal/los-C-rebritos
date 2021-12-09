CC = gcc
OUTFILE = kernel.out

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

makeshared:
	make -C ../Shared

debug: CFLAGS := $(CFLAGS) -g
debug: build

run-humedal: build
	./$(OUTFILE) "config/saludo-humedal.config"

run-mmu: build
	./$(OUTFILE) "config/mmu.config"

run-asignacion: build
	./$(OUTFILE) "config/asignacion.config"

run-tlb-fifo: build
	./$(OUTFILE) "config/tlb-fifo.config"

run-tlb-lru: build
	./$(OUTFILE) "config/tlb-lru.config"

run-planificacion-sjf: build
	./$(OUTFILE) "config/planificacion-sjf.config"

run-planificacion-hrrn: build
	./$(OUTFILE) "config/planificacion-hrrn.config"

run-deadlock: build
	./$(OUTFILE) "config/deadlock.config"

run-suspencion: build
	./$(OUTFILE) "config/suspencion.config"

run-batalla: build
	./$(OUTFILE) "config/batalla.config"

valgrind: debug build
	valgrind ./$(OUTFILE) --leak-check=full --track-origins=yes --log-file=valgrind.log

clean: 
	rm -rf obj
	rm -rf *.out
	make clean -C ../Shared
