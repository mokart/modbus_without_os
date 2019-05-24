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

/* ----------------------- Start implementation -----------------------------*/
//�û�Ӧ�ø�����ʹ�õ�Ӳ����ʼ����ʱ��ʱ����ʹ֮�ܲ����ж�ʱ��ΪusTim1Timerout50us*50us���жϡ�
//����ֵ���ΪTRUE
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_DeInit(TIM2);

	/*
	3.5���ַ�ʱ�����ֲ�ͬ��֡�������յ��������ַ�֮��ʱ����С��3.5���ַ�
	ʱ��ʱ��Ϊ��ͬһ��֡�ģ�����������3.5���ַ�ʱ������Ϊ�ǲ�ͬ֡��
	��һ��Ĵ���ͨ���У�����1���ַ���Ҫ��1λ��ʼλ��8λ����λ��1λУ��λ(����),
	1λֹͣλ,�ܹ� 1+8+1+1 = 11λ��3.5���ַ�ʱ����� 3.5 * 11 = 38.5λ��
	���粨������9600,��ô����1λ��ʱ����1000/9600 = 0.10416667(ms) ,
	������3.5���ַ�ʱ��ʹ�Լ�� 4 ms ,����ʱ����Ҫ���ж�ʱ��
	*/

	
  // �������Ԥ��Ƶϵ����7200/72M = 0.0001,��ÿ100us����ֵ��1
  //10us x 500 = 5ms,��5ms�ж�һ��	
  TIM_TimeBaseStructure.TIM_Period = 500;
  TIM_TimeBaseStructure.TIM_Prescaler = (7200 - 1);	//72000000/7200=10000
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
 	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, DISABLE);
	return TRUE;
}


//���� : ʹ�ܶ�ʱ������
//���� : �û���Ҫ�ڴ˺���������жϱ�־λ�����㶨ʱ������ֵ��������ʹ�ܶ�ʱ���ж�

void vMBPortTimersEnable(  )
{	
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM2, ENABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
}

//��������: ʹ�ܳ�ʱ��ʱ��
//����: �û���Ҫ�ڴ˺���������жϱ�־λ�����㶨ʱ��������ֵ�����رն�ʱ���ж�
void vMBPortTimersDisable(  )
{
	TIM_SetCounter(TIM2, 0);
	TIM_Cmd(TIM2, DISABLE);
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
}

//����: ��ʱ���жϺ���
//����: �˺��������޸ģ�ֻ�����û��Ķ�ʱ���ж��е��ô˺������ɣ�ͬʱ���û�Ӧ�ڵ��ô˺���������жϱ�־λ
void TIMERExpiredISR( void )
{
		OSIntEnter();		//�����ж�
	   //pxMBPortCBTimerExpired = xMBRTUTimerT35Expired;
    (void)pxMBPortCBTimerExpired();
    OSIntExit();        //���������л����ж�

}
