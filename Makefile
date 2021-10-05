all:
	make -C Kernel
	make -C MateLib
	make install -C Shared 
clean:
	make clean -C Kernel
