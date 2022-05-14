#include "mini_uart.h"
#include "peripheral/mmio.h"
#include "string.h"
typedef unsigned int uint32;
typedef unsigned char byte;

#define MAILBOX_BASE              MMIO_BASE + 0xb880

#define MAILBOX_READ              (uint32*)(MAILBOX_BASE)
#define MAILBOX_STATUS            (uint32*)(MAILBOX_BASE+0x18)
#define MAILBOX_WRITE             (uint32*)(MAILBOX_BASE+0x20)

#define MAILBOX_EMPTY             0x40000000
#define MAILBOX_FULL              0x80000000
#define MAILBOX_CODE_BUF_RES_SUCC 0x80000000
#define MAILBOX_CODE_BUF_REQ      0x00000000
#define MAILBOX_CODE_TAG_REQ      0x00000000

#define GET_ARM_MEMORY            0x00010005
#define GET_BOARD_REVISION        0x00010002
#define REQUEST_CODE              0x00000000
#define REQUEST_SUCCEED           0x80000000
#define REQUEST_FAILED            0x80000001
#define TAG_REQUEST_CODE          0x00000000
#define END_TAG                   0x00000000

// flag
#define MBOX_EMPTY                      0x40000000
#define MBOX_FULL                       0x80000000

// code
#define MBOX_CODE_BUF_REQ               0x00000000
#define MBOX_CODE_BUF_RES_SUCC          0x80000000
#define MBOX_CODE_TAG_REQ               0x00000000

// tag
#define MBOX_TAG_GET_BOARD_REVISION     0x00010002
#define MBOX_TAG_GET_VC_MEMORY          0x00010006
#define MBOX_TAG_SET_CLOCK_RATE         0x00038002
#define MBOX_TAG_SET_PHY_WIDTH_HEIGHT   0x00048003
#define MBOX_TAG_SET_VTL_WIDTH_HEIGHT   0x00048004
#define MBOX_TAG_SET_VTL_OFFSET         0x00048009
#define MBOX_TAG_SET_DEPTH              0x00048005
#define MBOX_TAG_SET_PIXEL_ORDER        0x00048006
#define MBOX_TAG_ALLOCATE_BUFFER        0x00040001
#define MBOX_TAG_GET_PITCH              0x00040008


void mailbox_call(uint32 *mailbox,byte channel){
  uint32 r = (uint32)(((unsigned long)mailbox) & (~0xF)) | (channel & 0xF);
  while (*MAILBOX_STATUS & MAILBOX_FULL) {
  }
  *MAILBOX_WRITE = r;
  while(1){
    while(*MAILBOX_STATUS & MAILBOX_EMPTY){
    }
    
    if (r == *MAILBOX_READ){
      return mailbox[1] == MAILBOX_CODE_BUF_RES_SUCC;
    }
  }
  return 0;
}

void get_board_revision(){
  uint32 __attribute__((aligned(16))) mailbox[7];
  mailbox[0] = 7 * 4; // buffer size in bytes
  mailbox[1] = REQUEST_CODE;
  // tags begin
  mailbox[2] = GET_BOARD_REVISION; // tag identifier
  mailbox[3] = 4; // maximum of request and response value buffer's length.
  mailbox[4] = TAG_REQUEST_CODE;
  mailbox[5] = 0; // value buffer
  // tags end
  mailbox[6] = END_TAG;

  mailbox_call(mailbox,8); // message passing procedure call, you should implement it following the 6 steps provided above.
  char s[11];
  i16toa(mailbox[5],s,8);
  uart_printf("Board Revision: 0x"); // it should be 0xa020d3 for rpi3 b+
  uart_printf(s);
  uart_printf("\n");
}

void get_arm_memory() {
    unsigned int __attribute__((aligned(16))) mailbox[8];
    mailbox[0] = 8 * 4;  // buffer size in bytes
    mailbox[1] = MAILBOX_CODE_BUF_REQ;
    // tags begin
    mailbox[2] = GET_ARM_MEMORY;          // tag identifier
    mailbox[3] = 8;                       // maximum of request and response value buffer's length.
    mailbox[4] = MAILBOX_CODE_TAG_REQ;    // tag code
    mailbox[5] = 0;                       // base address
    mailbox[6] = 0;                       // size in bytes
    mailbox[7] = 0x0;                     // end tag
    // tags end
    mailbox_call(mailbox, 8);
    uart_printf("ARM memory base addr: 0x%x size: 0x%x\n",mailbox[5],mailbox[6]);

}


unsigned int width, height, pitch, isrgb; /* dimensions and channel order */
unsigned char *fb;                        /* raw frame buffer address */

void fb_init() {
    unsigned int __attribute__((aligned(16))) mbox[35];

    mbox[0] = 35 * 4;
    mbox[1] = MBOX_CODE_BUF_REQ;

    // set physical width / height
    mbox[2] = MBOX_TAG_SET_PHY_WIDTH_HEIGHT;
    mbox[3] = 8;
    mbox[4] = MBOX_CODE_TAG_REQ;
    mbox[5] = 960;  // width in pixels
    mbox[6] = 720;  // height in pixels

    // set virtual width / height
    mbox[7] = MBOX_TAG_SET_VTL_WIDTH_HEIGHT;
    mbox[8] = 8;
    mbox[9] = MBOX_CODE_TAG_REQ;
    mbox[10] = 920;  // width in pixels
    mbox[11] = 720;  // height in pixels

    // set virtual offset
    mbox[12] = MBOX_TAG_SET_VTL_OFFSET;
    mbox[13] = 8;
    mbox[14] = MBOX_CODE_TAG_REQ;
    mbox[15] = 0;  // X in pixels
    mbox[16] = 0;  // Y in pixels

    // set depth
    mbox[17] = MBOX_TAG_SET_DEPTH;
    mbox[18] = 4;
    mbox[19] = MBOX_CODE_TAG_REQ;
    mbox[20] = 32;  // bits per pixel

    // set pixel order
    mbox[21] = MBOX_TAG_SET_PIXEL_ORDER;
    mbox[22] = 4;
    mbox[23] = MBOX_CODE_TAG_REQ;
    mbox[24] = 1;  // 0x0: BGR, 0x1: RGB

    // allocate buffer
    mbox[25] = MBOX_TAG_ALLOCATE_BUFFER;
    mbox[26] = 8;
    mbox[27] = MBOX_CODE_TAG_REQ;
    mbox[28] = 4096;  // req: alignment in bytes / res: frame buffer base address in bytes
    mbox[29] = 0;     // frame buffer size in bytes

    // get pitch
    mbox[30] = MBOX_TAG_GET_PITCH;
    mbox[31] = 4;
    mbox[32] = MBOX_CODE_TAG_REQ;
    mbox[33] = 0;  // res: bytes per line

    mbox[34] = 0x0;
    mailbox_call(mbox, 8);
    if (mbox[20] == 32 && mbox[28] != 0) {
        mbox[28] &= 0x3FFFFFFF;  // convert GPU address to ARM address
        width = mbox[5];         // get actual physical width
        height = mbox[6];        // get actual physical height
        pitch = mbox[33];        // get number of bytes per line
        isrgb = mbox[24];        // get the actual channel order
        fb = (void *)((unsigned long)mbox[28]);
    }
    else {
        uart_printf("Unable to set screen resolution to 1024x768x32\n");
    }
}

void fb_splash() {
    uart_printf("0x%x %x\n",(unsigned long long)fb>>32,fb);
    int x, y;
    unsigned char *ptr = fb;
    unsigned int white = 255 << 16 | 255 << 8 | 255;  // A B G R
    unsigned int black = 0;
    unsigned int current, start = black, spacing = 40;

    for (y = 0; y < height; y++) {
        if (y % spacing == 0 && y != 0) {
            start = (start == white) ? black : white;
        }
        current = start;
        for (x = 0; x < width; x++) {
            if (x % spacing == 0 && x != 0) {
                current = (current == white) ? black : white;
            }
            *((unsigned int *)ptr) = current;
            ptr += 4;
        }
    }
}
