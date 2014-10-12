#include <stdint.h>

class KeyboardDriver
{
public:
    void start();
private:
    bool initializeDevice();
    void PIC_remap(int offset1, int offset2);
    void outb(uint16_t port, uint8_t val);
    uint8_t inb(uint16_t port);
    void io_wait(void);
    void PIC_sendEOI(unsigned char irq);
    uint16_t __pic_get_irq_reg(int ocw3);
    uint16_t pic_get_irr(void);
    uint16_t pic_get_isr(void);
};
