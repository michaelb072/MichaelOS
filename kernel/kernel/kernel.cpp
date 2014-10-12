#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <drivers/keyboard.hpp>

/* Constructor for a conventional segment GDT (or LDT) entry */
/* This is a macro so it can be used in initializers */
#define GDT_ENTRY(flags, base, limit)           \
    ((((base)  & _AC(0xff000000,ULL)) << (56-24)) | \
     (((flags) & _AC(0x0000f0ff,ULL)) << 40) |  \
     (((limit) & _AC(0x000f0000,ULL)) << (48-16)) | \
     (((base)  & _AC(0x00ffffff,ULL)) << 16) |  \
     (((limit) & _AC(0x0000ffff,ULL))))

/* Simple and small GDT entries for booting only */

#define GDT_ENTRY_BOOT_CS   2
#define __BOOT_CS       (GDT_ENTRY_BOOT_CS * 8)

#define GDT_ENTRY_BOOT_DS   (GDT_ENTRY_BOOT_CS + 1)
#define __BOOT_DS       (GDT_ENTRY_BOOT_DS * 8)

#define GDT_ENTRY_BOOT_TSS  (GDT_ENTRY_BOOT_CS + 2)
#define __BOOT_TSS      (GDT_ENTRY_BOOT_TSS * 8)

#ifdef __ASSEMBLY__
#define _AC(X,Y)    X
#define _AT(T,X)    X
#else
#define __AC(X,Y)   (X##Y)
#define _AC(X,Y)    __AC(X,Y)
#define _AT(T,X)    ((T)(X))
#endif

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_early. */
#endif
void kernel_early(void)
{
    Tty::terminal_initialize();
}

/*
 * Set up the GDT
 */

struct gdt_ptr {
    unsigned char len;
    unsigned int ptr;
} __attribute__((packed));

static void setup_gdt(void)
{
    /* There are machines which are known to not boot with the GDT
       being 8-byte unaligned.  Intel recommends 16 byte alignment. */
    static const unsigned int boot_gdt[] __attribute__((aligned(16))) = {
        /* CS: code, read/execute, 4 GB, base 0 */
        [GDT_ENTRY_BOOT_CS] = GDT_ENTRY(0xc09b, 0, 0xfffff),
        /* DS: data, read/write, 4 GB, base 0 */
        [GDT_ENTRY_BOOT_DS] = GDT_ENTRY(0xc093, 0, 0xfffff),
        /* TSS: 32-bit tss, 104 bytes, base 4096 */
        /* We only have a TSS here to keep Intel VT happy;
           we don't actually use it for anything. */
        [GDT_ENTRY_BOOT_TSS] = GDT_ENTRY(0x0089, 4096, 103),
    };
    /* Xen HVM incorrectly stores a pointer to the gdt_ptr, instead
       of the gdt_ptr contents.  Thus, make it static so it will
       stay in memory, at least long enough that we switch to the
       proper kernel GDT. */
    static struct gdt_ptr gdt;

    gdt.len = sizeof(boot_gdt)-1;
    gdt.ptr = (unsigned int)&boot_gdt + (ds() << 4);

    asm volatile("lgdtl %0" : : "m" (gdt));
}


#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(void)
{
    setup_gdt();
    for (int i=0; i<115; i++)
        printf("Hello, kernel World! %d\n", i);
    KeyboardDriver keyboard;
    keyboard.start();
    printf("Keyboard Initialized\n");
}
