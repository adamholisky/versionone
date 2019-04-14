all:
	/v1/bin/nasm -fbin -o hello.o hello.asm
	/v1/bin/nasm -felf32 -o ../../build/hello_container.o hello_container.asm

clean:
	rm hello.o