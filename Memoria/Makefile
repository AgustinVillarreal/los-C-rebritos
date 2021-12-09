CC = gcc
OUTFILE = memoria.out

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

run-humedal: build
	./$(OUTFILE) "config/saludo-humedal.config"

run-mmu-lru: build
	./$(OUTFILE) "config/mmu-lru.config"

run-mmu-fifo: build
	./$(OUTFILE) "config/mmu-fifo.config"

run-asignacion-fija: build
	./$(OUTFILE) "config/asignacion-fija.config"

run-asignacion-dinamica: build
	./$(OUTFILE) "config/asignacion-dinamica.config"

run-tlb-fifo: build
	./$(OUTFILE) "config/tlb-fifo.config"

run-tlb-lru: build
	./$(OUTFILE) "config/tlb-lru.config"

run-swamp: build
	./$(OUTFILE) "config/swamp.config"

run-planificacion: build
	./$(OUTFILE) "config/planificacion.config"

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
