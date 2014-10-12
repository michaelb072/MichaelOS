#include <stdio.h>

#if defined(__is_michaelos_kernel)
#include <kernel/tty.h>
#endif

int putchar(int ic)
{
#if defined(__is_michaelos_kernel)
    char c = (char) ic;
    Tty::terminal_write(&c, sizeof(c));
#else
    // TODO: Implement a write system call.
#endif
    return ic;
}
