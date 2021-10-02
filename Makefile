all:
	make -C Kernel
	make -C MateLib
clean:
	make clean -C Kernel
