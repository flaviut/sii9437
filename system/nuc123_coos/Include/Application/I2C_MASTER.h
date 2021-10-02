#ifndef I2C_MASTER_H
#define I2C_MASTER_H

#include "NUC123Series.h"

typedef enum
{
    eI2C_DATA_SEND = 0,
    eI2C_DATA_RECV,
    eI2C_NUM_OF_PROC_TYPE
} eI2C_PROC_TYPE;

typedef enum
{
	I2C_STATUS__SUCCESS,
	I2C_STATUS__BUS_BUSY,
	I2C_STATUS__TIMEOUT,
	I2C_STATUS__NACK,
	I2C_STATUS__NONE,
} eI2C_Status_t;

void Init_I2C(void);
void I2C_Cmd_Data_Send(uint8_t *pu8CmdBuf, uint32_t u32CmdLen );
int I2C_Master(void);
void I2C_Port_Open(I2C_T *I2cPort, uint32_t u32BusClock);
BOOL sii_i2c_write(uint8_t devId, uint16_t offset, uint8_t offset_size,
		const uint8_t *pData, uint16_t len);
BOOL sii_i2c_read(uint8_t devId, uint16_t offset, uint8_t offset_size,
		uint8_t *pData, uint16_t len);

#endif
