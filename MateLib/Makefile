CC=gcc

build-example-lib:
	cd src && \
	$(CC) -c -Wall -I"../include" -fpic matelib.c && \
	$(CC) -shared matelib.o -o libmate.so && \
	rm matelib.o

build-example: build-example-lib
	$(CC) -g3 -Wall -L"./src" -I"include" main.c -lmate -lpthread -lcommons -lrt -o example

run: build-example
	cd examples/lib_usage && \
	LD_LIBRARY_PATH="../src/" ./example

clean:
	rm src/libmate.so example