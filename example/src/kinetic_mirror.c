/*
 * @brief I2C example
 * This example show how to use the I2C interface
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <stdlib.h>
#include <string.h>
#include <board.h>


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/
#define DEFAULT_I2C          I2C0

#define I2C_EEPROM_BUS       DEFAULT_I2C
#define I2C_IOX_BUS          DEFAULT_I2C

#define SPEED_100KHZ         100000
#define SPEED_400KHZ         400000

/** Own Slave address in Slave I2C device */
#define I2CDEV_S_OWN_ADDR   (0x90>>1)

/** Transfer Mode */
#define I2CDEV_TRANSFER_POLLING        1  /*0: interrupt mode, 1: polling mode */

static int mode_poll;   /* Poll/Interrupt mode flag */
static I2C_ID_T i2cDev = DEFAULT_I2C; /* Currently active I2C device */

/* EEPROM SLAVE data */
//#define I2C_SLAVE_EEPROM_SIZE       64
//#define I2C_SLAVE_EEPROM_ADDR       0x5A
#define I2C_SLAVE_TEMP_ADDR          0x24

/* Xfer structure for slave operations */
static I2C_XFER_T temp_xfer;
static I2C_XFER_T iox_xfer;

static uint8_t i2Cbuffer[4][256];


/** Max buffer length */
#define BUFFER_SIZE         0x10


uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"I2C demo \n\r"
"\t - MCU: LPC407x_8x \n\r"
"\t - Core: ARM CORTEX-M4 \n\r"
"\t - This example uses I2C as slave device to transfer data \n\r"
" to/from I2C master device \n\r"
"********************************************************************************\n\r";

/*****************************************************************************
 * Private functions
 ****************************************************************************/

void print_menu(void);
//void Buffer_Init(uint8_t type);

/*-------------------------PRIVATE FUNCTIONS------------------------------*/
/*********************************************************************//**
 * @brief       Print Welcome menu
 * @param[in]   none
 * @return      None
 **********************************************************************/
void print_menu(void)
{
    DEBUGOUT(menu1);
}

///*********************************************************************//**
// * @brief       Initialize buffer
// * @param[in]   type:
// *              - 0: Initialize Master_Buf with increment value from 0
// *                  Fill all member in Slave_Buf with 0
// *              - 1: Initialize Slave_Buf with increment value from 0
// *                  Fill all member in Master_Buf with 0
// *
// * @return      None
// **********************************************************************/
//void Buffer_Init(uint8_t type)
//{
//    uint32_t i;
//
//    if (type)
//    {
//        for (i = 0; i < BUFFER_SIZE; i++) {
//            Slave_Buf[i] = i;
//        }
//    }
//    else
//    {
//        for (i = 0; i < BUFFER_SIZE; i++) {
//            Slave_Buf[i] = 0;
//        }
//    }
//}
///*********************************************************************//**
// * @brief       Print buffer data
// * @param[in]   buff        Buffer address
// *                         size        Buffer size
// *
// * @return      None
// **********************************************************************/
//void Buffer_Print(uint8_t* buff, uint32_t size)
//{
//   uint32_t i = 0;
//    for (i = 0; i < size; i++) {
//        if (i%10 == 0){
//            _DBG_("");_DBG("    ");
//        }
//        else
//            _DBG(", ");
//        _DBH(buff[i]);
//        if( i ==  size - 1)
//            _DBG_("");
//    }
//    _DBG_("");
//}





/* State machine handler for I2C0 and I2C1 */
static void i2c_state_handling(I2C_ID_T id)
{
	if (Chip_I2C_IsMasterActive(id)) {
		Chip_I2C_MasterStateHandler(id);
	} else {
		Chip_I2C_SlaveStateHandler(id);
	}
}

/* Set I2C mode to polling/interrupt */
static void i2c_set_mode(I2C_ID_T id, int polling)
{
	if(!polling) {
		mode_poll &= ~(1 << id);
		Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandler);
		NVIC_EnableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
	} else {
		mode_poll |= 1 << id;
		NVIC_DisableIRQ(id == I2C0 ? I2C0_IRQn : I2C1_IRQn);
		Chip_I2C_SetMasterEventHandler(id, Chip_I2C_EventHandlerPolling);
	}
}

/* Initialize the I2C bus */
static void i2c_app_init(I2C_ID_T id, int speed)
{
	Board_I2C_Init(id);

	/* Initialize I2C */
	Chip_I2C_Init(id);
	Chip_I2C_SetClockRate(id, speed);

	/* Set default mode to interrupt */
	i2c_set_mode(id, 0);
}

static void i2c_read_setup(I2C_XFER_T *xfer, uint8_t addr, int numBytes)
{
	xfer->slaveAddr = addr;
	xfer->rxBuff = 0;
	xfer->txBuff = 0;
	xfer->txSz = 0;
	xfer->rxSz = numBytes;
	xfer->rxBuff = i2Cbuffer[1];

}


/*****************************************************************************
 * Public functions
 ****************************************************************************/
/**
 * @brief	SysTick Interrupt Handler
 * @return	Nothing
 * @note	Systick interrupt handler updates the button status
 */
void SysTick_Handler(void)
{
}

/**
 * @brief	I2C Interrupt Handler
 * @return	None
 */
void I2C1_IRQHandler(void)
{
	i2c_state_handling(I2C1);
}

/**
 * @brief	I2C0 Interrupt handler
 * @return	None
 */
void I2C0_IRQHandler(void)
{
	i2c_state_handling(I2C0);
}

/**
 * @brief	Main program body
 * @return	int
 */
int main(void)
{
//	print_menu();
	int tmp;
	int xflag = 0;
	Board_Init();
	SystemCoreClockUpdate();
	i2c_app_init(I2C0, SPEED_400KHZ);
	i2c_set_mode(I2C0, I2CDEV_TRANSFER_POLLING);
	i2cDev = I2C0;
	i2c_read_setup(&temp_xfer, (I2C_SLAVE_TEMP_ADDR << 1), 4);



	tmp = Chip_I2C_MasterRead(i2cDev, temp_xfer.slaveAddr, temp_xfer.rxBuff, 4);
	printf("Read %d bytes of data from slave 0x%02X.\r\n", tmp, temp_xfer.slaveAddr >> 1);
	char textBuff[7] = "     \n";
	memcpy(textBuff, i2Cbuffer, 4);
	printf("%X\n", i2Cbuffer[1][0]);
	Chip_I2C_DeInit(I2C0);


	return 0;
}
