CC=gcc

build-example-lib:
	cd src && \
	$(CC) -c -Wall -I"../include" -fpic utils.c && \
	$(CC) -c -Wall -I"../include" -fpic matelib.c && \
	$(CC) -shared utils.o matelib.o -o libmatelib.so && \
	rm *.o && \
	cp libmatelib.so /lib

build-example: build-example-lib
	$(CC) -g3 -Wall -L"./src" -I"include" main.c -lmatelib -lpthread -lcommons -lshared -lrt -o example

run: build-example
	c ./example "./config.config"

clean:
	rm src/libmatelib.so example

valgrind-example: build-example
	LD_LIBRARY_PATH="src/" valgrind --leak-check=full --track-origins=yes ./example "./config.config"

	