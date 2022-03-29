#include "aux.h"
#include "gpio.h"
#include "buffer.h"
#include "allocator.h"

int transmit_interrupt_open = 0;
char uart_buffer[1024];
unsigned int wr_buffer_index = 0;
unsigned int rd_buffer_index = 0;
struct buffer wbuffer,rbuffer;


void uart_init() {

    /* Initialize UART */
    *AUX_IRQ |= 1;       // Enable mini UART interrupt pending
    *AUX_ENABLES |= 1;   // Enable mini UART
    *AUX_MU_CNTL = 0;    // Disable TX, RX during configuration
    *AUX_MU_IER = 1;     // enable interrupt
    *AUX_MU_LCR = 3;     // Set the data size to 8 bit
    *AUX_MU_MCR = 0;     // Don't need auto flow control
    *AUX_MU_BAUD = 270;  // Set baud rate to 115200
    *AUX_MU_IIR = 6;     // No FIFO

    /* Map UART to GPIO Pins */

    // 1. Change GPIO 14, 15 to alternate function
    register unsigned int r = *GPFSEL1;  //gpio10~19 is located GPSEL1
    r &= ~((7 << 12) | (7 << 15));  // Reset GPIO 14, 15 
    r |= (2 << 12) | (2 << 15);     // Set ALT5
    *GPFSEL1 = r;

    // 2. Disable GPIO pull up/down (Because these GPIO pins use alternate functions, not basic input-output)
    // Set control signal to disable
    *GPPUD = 0;
    // Wait 150 cycles
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    // Clock the control signal into the GPIO pads
    *GPPUDCLK0 = (1 << 14) | (1 << 15);
    // Wait 150 cycles
    r = 150;
    while (r--) {
        asm volatile("nop");
    }
    // Remove the clock
    *GPPUDCLK0 = 0;

    // 3. Enable TX, RX
    *AUX_MU_CNTL = 3;

    
}

void uart_init_buffer(){
    wbuffer.start = 0;
    wbuffer.end = 0;
    rbuffer.start = 0;
    rbuffer.end = 0;
}

char uart_read() {
    // Check data ready field
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    // Read
    char r = (char)(*AUX_MU_IO);
    // Convert carrige return to newline
    return r == '\r' ? '\n' : r;
}

char uart_read_raw() {
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x01));
    return (char)(*AUX_MU_IO);
}

void uart_write(unsigned int c) {
    // Check transmitter idle field
    do {
        asm volatile("nop");
    } while (!(*AUX_MU_LSR & 0x20));
    // Write
    *AUX_MU_IO = c;
}

void uart_printf(char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '\n') uart_write('\r');
        else if(*fmt == '%'){
            fmt++;
            if(*fmt == 'd'){
                int arg =  __builtin_va_arg(args, int);
                char temp[10];
                itoa(arg,temp);
                uart_printf(temp);
            }else if(*fmt == 's'){
                char *arg =  __builtin_va_arg(args, char*);
                uart_printf(arg);
            }else if(*fmt == 'x'){
                int arg =  __builtin_va_arg(args, int);
                char temp[10];
                i16toa(arg,temp,8);
                uart_printf(temp);
            }else if(*fmt == 'c'){
                unsigned char arg = __builtin_va_arg(args, unsigned char);
                uart_write(arg);
            }
            else if(*fmt++ == 'l'){
                if(*fmt == 'e'){
                    unsigned int arg = __builtin_va_arg(args, unsigned int);
                    unsigned char *t = (unsigned char *) &arg;
                    for(int i=0;i<4;i++){
                        unsigned char temp[3];
                        temp[2]='\0';
                        i16toa(t[i],temp,2);
                        uart_printf(temp);
                    }
                }else if(*fmt == 'l'){
                    unsigned long long arg = __builtin_va_arg(args, unsigned long long);
                    unsigned char temp[24];
                    itoa(arg, temp);
                    uart_printf("%s",temp);
                }
            }
            fmt++;
            continue;
        }
        else if(*fmt == '\0') break;
        uart_write(*fmt++);
    }
}

void uart_flush() {
    while (*AUX_MU_LSR & 0x01) {
        *AUX_MU_IO;
    }
}

int uart_push(char c){
    return write_buffer(&wbuffer,c);
}

int uart_pop(unsigned char *c){
    return read_buffer(&rbuffer,c);
}

void handle_uart_irq()
{
    unsigned int id = *AUX_MU_IIR;
    if((id & 0x06) == 0x04)     //receive interrupt
	{
        if (*AUX_MU_IER == 1){
            *AUX_MU_IER = 3;
        }
        if( *AUX_MU_LSR & 0x01) {
            char c;
            c = *AUX_MU_IO & 0xFF;
            write_buffer(&rbuffer,c);
            uart_read_line();
        }
	}
    if((id & 0x06) == 0x02)   //transmit interrupt
	{
        if(*AUX_MU_LSR & 0x20) {
            unsigned char c;
            if(read_buffer(&wbuffer,&c) == 0) {
                // close transmit interrupt
                *AUX_MU_IER = 1; 
                return;
            }
            *AUX_MU_IO = c;
              
        }
	}
    return;
}
