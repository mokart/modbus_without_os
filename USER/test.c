#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h" 
#include "key.h" 
//ALIENTEK Mini STM32�����巶������3
//����ʵ��   
//����֧�֣�www.openedv.com
//������������ӿƼ����޹�˾
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 1
#define REG_INPUT_NREGS 20
#define REG_HOLDING_START 	1
#define REG_HOLDING_NREGS 	10
/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];    //REG_INPUT_NREGS 20�ֽ�
static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];//REG_HOLDING_NREGS 10�ֽ�
/* ----------------------- Start implementation -----------------------------*/


int main(void)
{

	Stm32_Clock_Init(9); //ϵͳʱ������
	delay_init(72);	     //��ʱ��ʼ�� 
	uart_init(72,9600);	 //���ڳ�ʼ��Ϊ9600
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ�    
  printf("uart test:\r\n");
	eMBInit(MB_RTU, 0x02, 0, 9600, MB_PAR_NONE);
	eMBEnable();
	
	while(1)
	{
		  //printf("main runing:\r\n");
		  LED0 = !LED0;
		  eMBPoll();
			delay_ms(1000);   
		
	}
}



/****************************************************************************
* ��	  �ƣ�eMBRegInputCB 
* ��    �ܣ���ȡ����Ĵ�������Ӧ�������� 04 eMBFuncReadInputRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺��λ�������� ֡��ʽ��: SlaveAddr(1 Byte)+FuncCode(1 Byte)
*								+StartAddrHiByte(1 Byte)+StartAddrLoByte(1 Byte)
*								+LenAddrHiByte(1 Byte)+LenAddrLoByte(1 Byte)+
*								+CRCAddrHiByte(1 Byte)+CRCAddrLoByte(1 Byte)
*							3 ��
****************************************************************************/
//����: ����Ĵ����غ�����
//���� : * pucRegBufferΪҪ��ӵ�Э���е����ݣ�usAddressΪ����Ĵ�����ַ��usNRegsΪҪ��ȡ�Ĵ����ĸ�����
//����: �û�Ӧ����Ҫ���ʵļĴ�����ַusAddress����Ӧ����Ĵ�����ֵ��˳����ӵ�pucRegBuffer�С�
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegHoldingCB 
* ��    �ܣ���Ӧ�������У�06 д���ּĴ��� eMBFuncWriteHoldingRegister 
*						  16 д������ּĴ��� eMBFuncWriteMultipleHoldingRegister
*						  03 �����ּĴ��� eMBFuncReadHoldingRegister
*						  23 ��д������ּĴ��� eMBFuncReadWriteMultipleHoldingRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��д�ļĴ�������
*						eMode: ������
* ���ڲ�����
* ע	  �⣺4 ��
****************************************************************************/
//���� : ���ּĴ����غ�����
//���� : * pucRegBufferΪҪЭ���е����ݣ�usAddressΪ����Ĵ�����ַ��
//       usNRegsΪ���ʼĴ����ĸ�����eModeΪ�������ͣ�MB_REG_READΪ�����ּĴ�����MB_REG_WRITEΪд���ּĴ�����
//���� : �û�Ӧ����Ҫ���ʵļĴ�����ַusAddress����Ӧ����Ĵ�����ֵ��˳����ӵ�pucRegBuffer�У�
//       ��Э���е����ݸ���Ҫ���ʵļĴ�����ַusAddress�ŵ���Ӧ���ּĴ����С�
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;


	if((usAddress >= REG_HOLDING_START)&&\
		((usAddress+usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		switch(eMode)
		{                                       
			case MB_REG_READ://�� MB_REG_READ = 0
       			 	while(usNRegs > 0)
					{
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] >> 8);            
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] & 0xFF); 
         			 iRegIndex++;
         			 usNRegs--;					
					}                            
        break;
			case MB_REG_WRITE://д MB_REG_WRITE = 1
				while(usNRegs > 0)
				{         
					  usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
			          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
			          iRegIndex++;
			          usNRegs--;
        }				
			}
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegCoilsCB 
* ��    �ܣ���Ӧ�������У�01 ����Ȧ eMBFuncReadCoils
*													05 д��Ȧ eMBFuncWriteCoil
*													15 д�����Ȧ eMBFuncWriteMultipleCoils
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: ��Ȧ��ַ
*						usNCoils: Ҫ��д����Ȧ����
*						eMode: ������
* ���ڲ�����
* ע	  �⣺��̵��� 
*						0 ��
****************************************************************************/
//���� : ��д��Ȧ�غ�����
//���� : * pucRegBufferΪҪ��ӵ�Э���е����ݣ�usAddressΪ��Ȧ��ַ��usNCoilsΪҪ������Ȧ�ĸ�����
//       eModeΪ�������ͣ�MB_REG_READΪ����Ȧ״̬��MB_REG_WRITEΪд��Ȧ����
//���� : �û�Ӧ����Ҫ���ʵ���Ȧ��ַusAddress����Ӧ��Ȧ��ֵ��˳����ӵ�pucRegBuffer�У�
//       ��Э���е����ݸ���Ҫ���ʵ���Ȧ��ַusAddress�ŵ���Ӧ��Ȧ�С�
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;


	if((usAddress >= REG_HOLDING_START)&&\
		((usAddress+usNCoils) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		switch(eMode)
		{                                       
			case MB_REG_READ://�� MB_REG_READ = 0
        while(usNCoils > 0)
				{
 					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] >> 8);            
 					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] & 0xFF); 
          			iRegIndex++;
          			usNCoils--;					
				}                            
        break;
			case MB_REG_WRITE://д MB_REG_WRITE = 1
				while(usNCoils > 0)
				{         
 					usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
           		usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
          			iRegIndex++;
          			usNCoils--;
        }				
			}
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}
/****************************************************************************
* ��	  �ƣ�eMBRegDiscreteCB 
* ��    �ܣ���ȡ��ɢ�Ĵ�������Ӧ�������У�02 ����ɢ�Ĵ��� eMBFuncReadDiscreteInputs
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNDiscrete: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺1 ��
****************************************************************************/
//���� : ����ɢ��Ȧ�غ���
//���� : * pucRegBufferΪҪ��ӵ�Э���е����ݣ�usAddressΪ��Ȧ��ַ��usNDiscreteΪҪ������Ȧ�ĸ�����
//
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}













