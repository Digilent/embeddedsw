/*
 * MIT License
 *
 * Copyright (c) 2019 Digilent
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *  Created on: Oct 28, 2019
 *      Author: Elod Gyorgy
 */

#include "spi_gpio.h"
#include "xspips.h"
#include "sleep.h"

#if defined(XPS_BOARD_GZU_3EG) || defined(XPS_BOARD_GZU_5EV)
//MCP23S08
#define SCLK_MAX_FREQ 10000000
#define MCP23S08_READ 1
#define MCP23S08_WRITE 0
#define MCP23S08_ADDRESS 0x20
#define RETRY_COUNT	10

struct prescale_t
{
	u32 div;
	u8 prescale;
};
static const struct prescale_t prescalers[] =
{
		{4, XSPIPS_CLK_PRESCALE_4},
		{8, XSPIPS_CLK_PRESCALE_8},
		{16, XSPIPS_CLK_PRESCALE_16},
		{32, XSPIPS_CLK_PRESCALE_32},
		{64, XSPIPS_CLK_PRESCALE_64},
		{128, XSPIPS_CLK_PRESCALE_128},
		{256, XSPIPS_CLK_PRESCALE_256}
};

static XSpiPs drv_inst;
static u8 send_buf_[8];
static u8 recv_buf_[8];

/*
 * @param dev_id SPI controller Device ID
 * @param ssid slave-select ID of the GPIO expander (supported 0-2)
 */
XStatus SpiGpioInit(u32 dev_id, u8 ss_id)
{
	XSpiPs_Config *SpiConfig;
	XStatus Status;

	if (ss_id > 2) return XST_FAILURE;

	/*
	 * Initialize the SPI driver so that it's ready to use
	 */
	SpiConfig = XSpiPs_LookupConfig(dev_id);
	if (NULL == SpiConfig) {
		return XST_FAILURE;
	}

	Status = XSpiPs_CfgInitialize(&drv_inst, SpiConfig,
				       SpiConfig->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to check hardware build
	 */
	Status = XSpiPs_SelfTest(&drv_inst);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the Spi device as a master
	 * MODE 0 is having issues with clock not going low before SS goes low
	 * MODE 3 (CPOL=1, CPHA=1) works fine
	 */
	XSpiPs_SetOptions(&drv_inst, XSPIPS_MASTER_OPTION | XSPIPS_FORCE_SSELECT_OPTION
			| XSPIPS_CLK_ACTIVE_LOW_OPTION | XSPIPS_CLK_PHASE_1_OPTION);


	size_t i;
	for (i=0; i<sizeof(prescalers)/sizeof(prescalers[0]); ++i)
	{
		if (SpiConfig->InputClockHz / prescalers[i].div <= SCLK_MAX_FREQ)
			break;
	}

	if (i>=sizeof(prescalers)/sizeof(prescalers[0]))
	{
		return XST_FAILURE;
	}

	XSpiPs_SetClkPrescaler(&drv_inst, prescalers[i].prescale);

	XSpiPs_SetSlaveSelect(&drv_inst, ss_id);

	u8 ipol;
	//IPOL register should read 0x00 after reset
	if (XST_SUCCESS != (Status = SpiGpioReadReg(MCP23S08_REG_IPOL, &ipol)))
		return Status;
	if (ipol != 0x00)
		return XST_FAILURE;

	return XST_SUCCESS;
}
/*
 * @param reg is the register address to read
 * @param value is a pointer to the variable to store the value read
 * @returns 0 on success, non-zero on failure
 */
XStatus SpiGpioReadReg(u8 reg, u8* value)
{
	u32 timeout = RETRY_COUNT;
	XStatus Status;

	memset(send_buf_, 0xFF, sizeof(recv_buf_));
	memset(recv_buf_, 0xFF, sizeof(recv_buf_));
	send_buf_[0] = (MCP23S08_ADDRESS << 1) | MCP23S08_READ;
	send_buf_[1] = reg;

	while (XST_DEVICE_BUSY == (Status = XSpiPs_PolledTransfer(&drv_inst, send_buf_, recv_buf_, 3)) && --timeout)
	{
		usleep(1000);
	}
	if (!timeout)
	{
		return XST_FAILURE;
	}

	*value = recv_buf_[2];
	return XST_SUCCESS;
}
/*
 * @param reg is the register address to write
 * @param value is the value write
 * @returns 0 on success, non-zero on failure
 */
XStatus SpiGpioWriteReg(u8 reg, u8 value)
{
	u32 timeout = RETRY_COUNT;
	XStatus Status;

	memset(send_buf_, 0xFF, sizeof(recv_buf_));
	memset(recv_buf_, 0xFF, sizeof(recv_buf_));
	send_buf_[0] = (MCP23S08_ADDRESS << 1) | MCP23S08_WRITE;
	send_buf_[1] = reg;
	send_buf_[2] = value;

	while (XST_DEVICE_BUSY == (Status = XSpiPs_PolledTransfer(&drv_inst, send_buf_, recv_buf_, 3)) && --timeout)
	{
		usleep(1000);
	}
	if (!timeout)
	{
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
#endif//defined(XPS_BOARD_GZU_3EG) || defined(XPS_BOARD_GZU_5EV)
