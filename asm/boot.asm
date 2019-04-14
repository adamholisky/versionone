global start_one
extern kernel_main
global load_kernel_managed_paging
 
; Multiboot Header Settings
MBALIGN             equ  1<<0   ; Align loaded modules on page boundaries
MEMINFO             equ  1<<1   ; Give memory map
MBOOT_VBE_MODE      equ  1<<2   ; Give us control with video mode already set
%if D_ENABLE_GRAPHICS = 1
    FLAGS    equ  MBALIGN | MEMINFO | MBOOT_VBE_MODE
%else
    FLAGS    equ  MBALIGN | MEMINFO
%endif

MAGIC    equ  0x1BADB002        ; Multiboot magic number
CHECKSUM equ -(MAGIC + FLAGS)   ; Checksum for multiboot
 
; This is the virtual base address of kernel space.
KERNEL_VIRTUAL_BASE equ 0xC0000000                  ; 3GB
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; Page directory index of kernel's 4MB PTE.
 
 
section .data
align 0x1000

BootPageDirectory:
    ; bit 7: PS The kernel page is 4MB.
    ; bit 1: RW The kernel page is read/write.
    ; bit 0: P  The kernel page is present.
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0                 ; Pages before kernel space.
    ; This page directory entry defines a 4MB page containing the kernel.
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; Pages after the kernel image.

GDTR:
    DW GDT_END-GDT-1
    DD GDT

GDT:
NULL_SEL         EQU $-GDT      ; null descriptor is required (64bit per entry)
      DD 0x0
      DD 0x0
CODESEL          EQU $-GDT      ; 4GB Flat Code at 0x0 with max 0xFFFFF limit
      DW     0xFFFF                 ; Limit(2):0xFFFF
      DW     0x0                    ; Base(3)
      DB     0x0                    ; Base(2)
      DB     0x9A                   ; Type: present,ring0,code,exec/read/accessed (10011000)
      DB     0xCF                   ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
      DB     0x0                    ; Base(1)
DATASEL          EQU $-GDT      ; 4GB Flat Data at 0x0 with max 0xFFFFF limit
      DW     0xFFFF                 ; Limit(2):0xFFFF
      DW     0x0                    ; Base(3)
      DB     0x0                    ; Base(2)
      DB     0x92                   ; Type: present,ring0,data/stack,read/write (10010010)
      DB     0xCF                   ; Limit(1):0xF | Flags:4Kb inc,32bit (11001111)
      DB     0x0                    ; Base(1)
GDT_END:
 
section .text

MultiBootHeader:
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

    dd 0
    dd 0
    dd 0
    dd 0
    dd 0

%if D_ENABLE_GRAPHICS = 1
    dd 0                                ; Normal video mode
    dd 1280                             ; Width
    dd 720                              ; Height
    dd 32                               ; Color depth
%endif
 
STACKSIZE equ 0x4000
  
start_one:
	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx                        ; Load Page Directory Base Register.

	mov ecx, cr4
	or ecx, 0x00000010                  ; Set PSE bit in CR4 to enable 4MB pages.
	mov cr4, ecx

	mov ecx, cr0
	or ecx, 0x80000000                  ; Set PG bit in CR0 to enable paging.
	mov cr0, ecx

	lea ecx, [start_two]                ; Start fetching instructions in kernel space via jump
	jmp ecx

start_two:
	mov dword [BootPageDirectory], 0    ; Unmap the identity-mapped first 4MB of physical address space
	invlpg [0]

	mov esp, stack+STACKSIZE            ; set up the stack
	cli
	lgdt [GDTR]

	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	jmp 0x08:start_three

start_three:
	add  ebx, KERNEL_VIRTUAL_BASE
	mov  ecx, KERNEL_PAGE_NUMBER
	push ecx							; Push KERNEL_PAGE_NUMBER
	push ebx							; Push multiboot struct from GRUB
	push eax							; Push multiboot magic from GRUB
	call kernel_main                    ; Enter C
	hlt                                 ; halt machine should kernel return

load_kernel_managed_paging:
    push ebp
        mov ebp, esp
        
        mov eax, [esp + 8]
        sub eax, KERNEL_VIRTUAL_BASE
        mov cr3, eax

        ;invlpg [0]

        mov esp, ebp
    pop ebp
    ret

section .bss
align 32
stack:
    resb STACKSIZE      ; reserve 16k stack on a uint64_t boundary