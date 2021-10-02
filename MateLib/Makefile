CC=gcc

build-example-lib:
	cd src && \
	$(CC) -c -Wall -I"../include" -fpic utils.c && \
	$(CC) -c -Wall -I"../include" -fpic matelib.c && \
	$(CC) -shared matelib.o utils.o -o libmate.so && \
	rm *.o

build-example: build-example-lib
	$(CC) -g3 -Wall -L"./src" -I"include" main.c -lmate -lpthread -lcommons -lshared -lrt -o example

run: build-example
	LD_LIBRARY_PATH="src/" ./example

clean:
	rm src/libmate.so example