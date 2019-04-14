align 4

global call_from_c_test

global int22
global int23
global int24
global int25
global int26
global int27
global int28
global int29
global int2A
global int2B
global int2C
global int2D
global int2E
global int2F

extern int_22
extern int_23
extern int_24
extern int_25
extern int_26
extern int_27
extern int_28
extern int_29
extern int_29
extern int_2A
extern int_2B
extern int_2D
extern int_2E
extern int_2F

extern call_from_asm_test
extern interrupt_default_handler
extern keyboard_interrupt_handler
extern mouse_handler
extern ata_primary_irq
extern ata_secondary_irq

section .text

global sys_break
sys_break:
     mov eax, 0x1000
     int 0x30
     ret

call_from_c_test:
	mov eax, 0xBADA
     int 0x30
	ret


[global int20]
int20:
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax, 0x10    ; Data segment
               mov ds,eax
               mov es,eax
               mov eax, 0x1001
               push eax
                    cld
                    call interrupt_default_handler   ; Test Interrupt Handler
               pop eax
          pop gs
          pop fs
          pop es
          pop ds
     popa
     iret

[global int21]
int21:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call keyboard_interrupt_handler
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int22:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_22
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int23:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_23
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int24:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_24
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int25:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_25
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int26:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_26
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int27:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_27
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int28:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_28
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int29:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_29
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int2A:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_2A
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int2B:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_2B
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int2C:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call mouse_handler
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int2D:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call int_2D
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int2E:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call ata_primary_irq
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

int2F:
     cli
     pusha
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               cld
               call ata_secondary_irq
          pop gs
          pop fs
          pop es
          pop ds
     popa
     sti
     iret

[global int30]
int30:
     pushad
          push ds
          push es
          push fs
          push gs
               mov eax,0x10    ; Data segment
               mov ds,eax
               mov es,eax
               mov eax, 0x0
               push eax
                    cld
                    call interrupt_default_handler   ; Default Interrupt Handler
               pop eax
          pop gs
          pop fs
          pop es
          pop ds
     popad
     iret

; interrupts.asm
; contains all system exception interrupts
; and other system interface interrupts
; note: exception 15 and 20-31 are Intel Reserved

[extern int_00]
[global int00]
int00:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_00    ; Divide by Zero #DE
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_01]
[global int01]
int01:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_01    ; Debug #DB
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_02]
[global int02]
int02:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_02    ; NMI interrupt #--
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_03]
[global int03]
int03:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_03    ; Breakpoint #BP
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_04]
[global int04]
int04:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_04    ; Overflow #OF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_05]
[global int05]
int05:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_05    ; Bound Range Exception #BR
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_06]
[global int06]
int06:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_06    ; Invalid Opcode #UD
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_07]
[global int07]
int07:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_07    ; Device Not Available #NM
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_08]
[global int08]
int08:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_08    ; Double Fault #DF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret


[extern int_09]
[global int09]
int09:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_09    ; Coprocessor Segment Overrun
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_10]
[global int10]
int10:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_10    ; Invalid Task State Segment (TSS) #TS
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_11]
[global int11]
int11:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_11    ;  Segment Not Present #NP
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_12]
[global int12]
int12:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_12    ; Stack Segment Fault #SS
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_13]
[global int13]
int13:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_13    ; General Protection Fault (GPF) #GP
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_14]
[global int14]
int14:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     mov eax,cr2
     push eax
     call int_14    ; Page Fault #PF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

;; Exception 15 is an Intel Reserved Interrupt

[extern int_16]
[global int16]
int16:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_16    ; x87 Floating-Point Error #MF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_17]
[global int17]
int17:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_17    ; Alignment Check #AC
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_18]
[global int18]
int18:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_18    ; Machine Check #MC
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

[extern int_19]
[global int19]
int19:
     pusha
     push ds
     push es
     push fs
     push gs
     mov eax,0x10    ; Data segment
     mov ds,eax
     mov es,eax
     cld
     call int_19    ; SIMD Floating-Point Exception #XF
     pop gs
     pop fs
     pop es
     pop ds
     popa
     iret

;; Exceptions 20-31 are Intel Reserved Interrupts

;; E n d   o f   E x c e p t i o n s   I n t e r r u p t s
