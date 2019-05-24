/*
  * FreeModbus Libary: LPC214X Port
  * Copyright (C) 2007 Tiago Prado Lone <tiago@maxwellbohr.com.br>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

#include "port.h"
#include "usart2.h"	
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/* ----------------------- Start implementation -----------------------------*/
//使能串口的接收 发送功能
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	if(TRUE==xRxEnable)
	{
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);	
	}

	if(TRUE==xTxEnable)
	{
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
	else
	{
	   USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
}

//关闭串口的发送使能及接收使能
void vMBPortClose( void )
{
	USART_ITConfig(USART2, USART_IT_TXE|USART_IT_RXNE, DISABLE);
	USART_Cmd(USART2, DISABLE);
}

//初始化串口的端口 波特率 数据位 奇偶校验
//RTU模式则有ucDataBits=8.
BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  usart2_init(ulBaudRate);
	return TRUE;
}



BOOL xMBPortSerialPutByte( CHAR ucByte )
{
	USART_SendData(USART2, ucByte);
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)/*等待发送完成*/
  {
  
  }		
	return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
	*pucByte = USART_ReceiveData(USART2);
	return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
	  //pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM;
    pxMBFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
	  //pxMBFrameCBByteReceived = xMBRTUReceiveFSM;
    pxMBFrameCBByteReceived();
}
