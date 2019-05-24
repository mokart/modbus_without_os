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
 * File: $Id: porttimer.c,v 1.1 2007/04/24 23:15:18 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "sys.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM4_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=(1<<2);	          //TIM6ʱ��ʹ��    
 	TIM4->ARR=arr;  	              //�趨�������Զ���װֵ 
	TIM4->PSC=psc-1;  	            //Ԥ��Ƶ������
	TIM4->DIER|=(1<<0);             //��������ж�				
	TIM4->CR1&=~(1<<0);             //��ֹ��ʱ��6
  MY_NVIC_Init(1,3,TIM4_IRQn,2);  //��ռ1�������ȼ�3����2									 
}

//5ms
void Tim4_Clear_Enable(void)
{
		TIM4->CR1&=~(1<<0);     //���ö�ʱ��4
    TIM4->CNT = 0;          //�������
	 	TIM4->ARR=50;  	        //�趨�������Զ���װֵ 
	  TIM4->PSC=7200-1;  	    //Ԥ��Ƶ������
		TIM4->SR&=~(1<<0);      //����жϱ�־λ
	  TIM4->DIER|=(1<<0);     //��������ж�				
		TIM4->CR1|=(1<<0);      //ʹ�ܶ�ʱ��6
}


void Tim4_Clear_Disable(void)
{
	  
    TIM4->CNT = 0;             //�������
		TIM4->SR&=~(1<<0);         //����жϱ�־λ
		TIM4->DIER &= ~(1<<0);     //��ֹ�����ж�
		TIM4->CR1&=~(1<<0);        //���ö�ʱ��6

}

/* ----------------------- Start implementation -----------------------------*/
//�û�Ӧ�ø�����ʹ�õ�Ӳ����ʼ����ʱ��ʱ����ʹ֮�ܲ����ж�ʱ��ΪusTim1Timerout50us*50us���жϡ�
//����ֵ���ΪTRUE
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	TIM4_Int_Init(50,7200-1);  //��ʱ5ms
	return TRUE;
}


//���� : ʹ�ܶ�ʱ������
//���� : �û���Ҫ�ڴ˺���������жϱ�־λ�����㶨ʱ������ֵ��������ʹ�ܶ�ʱ���ж�
void vMBPortTimersEnable()
{	
    Tim4_Clear_Enable();
}

//��������: ʹ�ܳ�ʱ��ʱ��
//����: �û���Ҫ�ڴ˺���������жϱ�־λ�����㶨ʱ��������ֵ�����رն�ʱ���ж�
void vMBPortTimersDisable()
{
    Tim4_Clear_Disable();
}

//����: ��ʱ���жϺ���
//����: �˺��������޸ģ�ֻ�����û��Ķ�ʱ���ж��е��ô˺������ɣ�ͬʱ���û�Ӧ�ڵ��ô˺���������жϱ�־λ
void TIMERExpiredISR(void)
{

  	//OSIntEnter();		//�����ж�
	  //pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;
    (void)pxMBPortCBTimerExpired();
    //OSIntExit();        //���������л����ж�

}

void TIM4_IRQHandler(void)
{
	
	if(TIM4->SR&0X0001)//����ж�
	{
	    TIMERExpiredISR();
			TIM4->SR&=~(1<<0);   //����жϱ�־λ
	}
	
}


