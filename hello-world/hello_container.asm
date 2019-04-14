; hello_contaimer.asm
;
; A container assembly to allow the GNU linker to place the binary code in hello.o at the required memory location.
;
; Copyright 2017 Adam Holisky
;
; Compile with the command "make" in this directory.
;
section .hellocode	; GNU linker will look fro this section of code and include it outright
incbin "hello.o"	; This includes the raw bin file, no questions asked

section .code		; NASM complains if we don't have a code section
asection:			; Give out code section a name that won't be used elsewhere in the OS, for safety
	nop 			; Our one command... do nothing