[bits 16]
; Switch to protected mode
switch_to_pm:
                        ; We must switch off interrupts until we have
    cli                 ; set-up the protected mode interrupt vector
                        ; otherwise interrupts will run rito

lgdt [gdt_descriptor]   ; Load our global descriptor table, which defines
                        ; the protected mode segments (e.g. for code and data)

mov eax, cr0            ; To make the switch to protected mode, we set
or eax, 0x1             ; the first bit of CR0, a control register
mov cr0, eax

jmp CODE_SEG:init_pm    ; Make a far jump (i.e. to a new segment) to our 32-bit
                        ; code. This also forces the CPU to flush it's cache of
                        ; pre-fetched and real-mode decoded instructions, which can
                        ; cause problems

[bits 32]
; Initialize registers and the stack once in PM
init_pm:
    mov ax, DATA_SEG    ; Now in PM, our old segments are meaningless
    mov ds, ax          ; so we point our segment registers to the
    mov ss, ax          ; data selector we defined in out GDT
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000    ; Update our stack position so it is right
    mov esp, ebp        ; at the top of the free space.

    call BEGIN_PM       ; Finally, call some well-known label

[bits 32]
BEGIN_PM:
    mov ebx, MSG_PROT_MODE
    call print_string_pm    ; Use our 32-bit print routine

    jmp $                   ; Hane

