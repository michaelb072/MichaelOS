#include <drivers/keyboard.hpp>
#include <drivers/keyboard_constants.hpp>
#include <stdio.h>

void KeyboardDriver::start()
{
    initializeDevice();
    unsigned int i = 0;
    while (true)
    {
        if (i == 0)
        {
            printf("received: %d on PIC1_DATA\n", inb(PIC1_DATA));
            printf("received: %d on PIC2_DATA\n", inb(PIC2_DATA));
        }
        i++;
    }
}

bool KeyboardDriver::initializeDevice()
{
    PIC_remap(PIC1, PIC2);
    return true;
}

void KeyboardDriver::PIC_sendEOI(unsigned char irq)
{
    if(irq >= 8)
        outb(PIC2_COMMAND,PIC_EOI);

    outb(PIC1_COMMAND,PIC_EOI);
}

void KeyboardDriver::PIC_remap(int offset1, int offset2)
{
    unsigned char a1, a2;

    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT+ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT+ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);                 // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, offset2);                 // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 4);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}


// Basic port I/O
inline void KeyboardDriver::outb(uint16_t port, uint8_t val)
{
    // asm volatile( "outb %0, %1" : : "a"(val), "Nd"(port) );
	asm volatile("outb %0,%1" : : "a" (val), "dN" (port));
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
}

inline uint8_t KeyboardDriver::inb(uint16_t port)
{
    uint8_t ret;
    // asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
	asm volatile("inb %1,%0" : "=a" (ret) : "dN" (port));
    /* TODO: Is it wrong to use 'N' for the port? It's not a 8-bit constant. */
    /* TODO: Should %1 be %w1? */
    return ret;
}

inline void KeyboardDriver::io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    /* TODO: Is there any reason why al is forced? */
}

/* Helper func */
uint16_t KeyboardDriver::__pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t KeyboardDriver::pic_get_irr(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t KeyboardDriver::pic_get_isr(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}
