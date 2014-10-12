print_string:
    pusha
    mov al, [bx]
    lodsb
    cmp al, 0
    int 0x10
    jnz print_string
    mov al, 10
    int 0x10
    mov al, 13
    int 0x10
    popa
    ret

; Define some constants
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

; prints a null - terminated string pointed to by EDX
print_string_pm :
    pusha
    mov edx, VIDEO_MEMORY ; Set edx to the start of vid mem.

