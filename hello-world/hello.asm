; hello.asm
; 
; A Hello, World written for the versionONE operating system.
;
; Copyright 2017 Adam Holisky
;
; Compile by running "make" in this directory
;
section .code

main:
    mov edx, my_message 			; printf() expects a pointer ot the text in the edx register
    mov eax, 0x0001					; printf() is system command 0x0001

    int 0x30						; Call the system command

    ret 							; End the program

section .data

my_message: db 'I bet this is going to crash infront of you all.',10,0	;Defines the string, followed by a new line, null terminated


; Equilivent linux hello.asm:
; 
; section     .text
; global      _start                              ;must be declared for linker (ld)
;
; _start:                                         ;tell linker entry point

;    mov     edx,len                             ;message length
;    mov     ecx,msg                             ;message to write
;    mov     ebx,1                               ;file descriptor (stdout)
;    mov     eax,4                               ;system call number (sys_write)
;    int     0x80                                ;call kernel

;    mov     eax,1                               ;system call number (sys_exit)
;    int     0x80                                ;call kernel

; section     .data
;
; msg     db  'Hello, world!',0xa                 ;our dear string
; len     equ $ - msg                             ;length of our dear string