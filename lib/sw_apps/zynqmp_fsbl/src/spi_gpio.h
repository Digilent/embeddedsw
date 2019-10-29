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

#ifndef SRC_SPI_GPIO_H_
#define SRC_SPI_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "xfsbl_hw.h"

#if defined(XPS_BOARD_GZU_3EG) || defined(XPS_BOARD_GZU_5EV)
#include "xstatus.h"

#define SPI_GPIO_DEVICE_ID XPAR_PSU_SPI_0_DEVICE_ID
#define MCP23S08_REG_IODIR 0x00
#define MCP23S08_REG_IPOL 0x01
#define MCP23S08_REG_PORT 0x09

XStatus SpiGpioInit(u32 dev_id, u8 ss_id);
XStatus SpiGpioReadReg(u8 reg, u8* value);
XStatus SpiGpioWriteReg(u8 reg, u8 value);
#endif /* XPS_BOARD_GZU_3EG || XPS_BOARD_GZU_5EV */

#ifdef __cplusplus
}
#endif

#endif /* SRC_SPI_GPIO_H_ */
