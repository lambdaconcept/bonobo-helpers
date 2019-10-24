#ifndef __BONOBO_H__
#define __BONOBO_H__

/* Multiplexer */

#define MUX_MODE_NC                 0
#define MUX_MODE_SDQ                1
#define MUX_MODE_SWD                2
#define MUX_MODE_ACCRESET           3

/* SDQ Core */

#define SDQ_MODE_IDLE               0
#define SDQ_MODE_SNR                2
#define SDQ_MODE_SERIAL             3
#define SDQ_MODE_SWD                4
#define SDQ_MODE_RESET              5

#define SDQ_STATUS_DONE             (1 << 0)
#define SDQ_STATUS_TIMEOUT          (1 << 1)
#define SDQ_STATUS_PRESENT          (1 << 2)

/* SWD Core */

#define SWDCORE_PARITYOK    0x10
#define SWDCORE_DONE        0x08
#define SWDCORE_OK          0x04
#define SWDCORE_ACK_MASK    0x07

/* USB Protocol Header */

#define CMD_SWD_RESET       0
#define CMD_SWD_JTAG2SWD    1
#define CMD_SWD_SWD2JTAG    2
#define CMD_SWD_EXEC        3

#define CMD_SWD_FREQ        4
#define CMD_MUX_SEL         5
#define CMD_SDQ_DEFAULT     6
#define CMD_SDQ_SEL         7
#define CMD_SDQ_RESULT      8

/* USB */

#define VID  0xffff
#define PID  0x1234

#define BULK_TRANSFER_SIZE  512
#define BULK_EP_IN      (1 | LIBUSB_ENDPOINT_IN)
#define BULK_EP_OUT     (2 | LIBUSB_ENDPOINT_OUT)
#define BULK_EP_TIMEOUT 1000

#endif /* __BONOBO_H__ */
