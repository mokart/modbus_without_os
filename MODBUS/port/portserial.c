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

//ʹ�ܴ��ڵĽ��� ���͹���
void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	if(TRUE==xRxEnable)
	{
		USART2->CR1|=(1<<5);     //���շǿ�ʹ��
	}
	else
	{
		USART2->CR1&=~(1<<5);    //���շǿս��� 		
	}

	if(TRUE==xTxEnable)
	{
		//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		USART2->CR1|=(1<<7);    //���Ϳ�	    		
	}
	else
	{
		USART2->CR1&=~(1<<7);    //���Ϳս��� 		
	}
}

//�رմ��ڵķ���ʹ�ܼ�����ʹ��
void vMBPortClose( void )
{
	//USART_ITConfig(USART2, USART_IT_TXE|USART_IT_RXNE, DISABLE);
	//USART_Cmd(USART2, DISABLE);
}


//ֻ�д���1��72MHZ
//��Ĵ��ڶ���36MHZ
void uart2_init(u32 pclk2,u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)(pclk2*1000000)/(bound*16);//�õ�USARTDIV
	mantissa=temp;				                 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������	 
  mantissa<<=4;
	mantissa+=fraction; 
	RCC->APB2ENR|=(1<<2);   //ʹ��PORTA��ʱ��  
	RCC->APB1ENR|=(1<<17);  //ʹ�ܴ���ʱ�� 
	GPIOA->CRL&=0XFFFF00FF;//IO״̬����  //PA2TX PA3RX
	GPIOA->CRL|=0X00008B00;//IO״̬����
		  
	RCC->APB1RSTR|=(1<<17);   //��λ����2
	RCC->APB1RSTR&=~(1<<17);//ֹͣ��λ	   	   
	//����������
 	USART2->BRR=mantissa; // ����������	 
	USART2->CR1|=0X200C;  //1λֹͣ,��У��λ.
	//ʹ�ܽ����ж�
	//USART2->CR1|=1<<8;    //PE�ж�ʹ��
	USART2->CR1|=1<<5;    //���ջ������ǿ��ж�ʹ��	  
  //	USART2->CR1|=1<<7;    //���Ϳ�
	MY_NVIC_Init(3,3,USART2_IRQn,2);//��2��������ȼ� 

}


//��ʼ�����ڵĶ˿� ������ ����λ ��żУ��
//RTUģʽ����ucDataBits=8.
BOOL xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  uart2_init(36,ulBaudRate);
	
	return TRUE;
}



BOOL xMBPortSerialPutByte( CHAR ucByte )
{
	while((USART2->SR&0X40)==0);//ѭ������,ֱ���������   
	USART2->DR = (u8) ucByte;      
	return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
	*pucByte = USART2->DR; 
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


void USART2_IRQHandler(void)
{
	

	if(USART2->SR&(1<<5))//���յ�����
	{
		prvvUARTRxISR();
	}
	
	if(USART2->SR&(1<<3))//���յ�����
	{
    prvvUARTRxISR();
  }
	
	if(USART2->SR&(1<<7))//���Ϳ�
	{
		prvvUARTTxReadyISR();
	}
}
