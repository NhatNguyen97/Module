#define REG_SIZE 1
#define NUM_CTRL_REGS 1
#define NUM_STS_REGS 5
#define NUM_DATA_REGS 256
#define NUM_DEV_REGS (NUM_CTRL_REGS + NUM_STS_REGS + NUM_DATA_REGS)
/* Decribe status register */
/* START ....................*/
/* register READ */
/* [READ_COUNT_H_REG : READ_COUNT_L_REG] */
#define READ_COUNT_H_REG 0
#define READ_COUNT_L_REG 1

/* Similar READ -> WRITE */
#define WRITE_COUNT_H_REG 2
#define WRITE_COUNT_L_REG 3

/* register DEVICE */
#define DEVICE_STATUS_REG 4

#define STS_READ_ACCESS_BIT (1 << 0)
#define STS_WRITE_ACCESS_BIT (1 << 1)
#define STS_DATAREGS_OVERFLOW_BIT (1 << 2)

#define READY 1
#define NOT_ONLY 0
#define OVERFLOW 1
#define NOT_OVERFLOW 0 
/* END ..........................*/

/* Decribe control register */
/* CONTROL_ACCESS_REG */
/* START ....................*/
#define CONTROL_ACCESS_REG 0
#define CTRL_READ_DATA_BIT (1 << 0)
#define CTRL_WRITE_DATA_BIT (1 << 1)

#define ENABLE 1
#define DISABLE 0
/* END ....................*/
