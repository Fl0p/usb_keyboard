/*******************************************************************************
* File Name          : main
* Author             : luohui
* Date               : 21-08-2014
* E-mail             : luohui2320@gmail.com
* Description        : 	
											����STM32��USB��ʵ�� USB_keyboard����
********************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include <math.h>
#include <stdio.h>
#include "USART.h"
#include "rotary_encoder.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t PrevXferComplete = 1;
INT8U key_flag = 0x41;
INT8U flag = 0;

extern INT8U rotary_detect_flag;
extern INT8S rotary_dir[4][4];
extern INT8U rotary_dir_flag[3];
extern struct rotary_data rdata;


INT8U Send_Buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void System_Init(void);
void Key_Handler(void);

/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
  Set_System();
	
  System_Init();
  
  USB_Interrupts_Config();
  
  Set_USBClock();
  
  USB_Init();
	
  while (1)
  {
    if (bDeviceState == CONFIGURED)
    {
      if(PrevXferComplete && (flag | rotary_detect_flag))
      {
				
				Key_Handler();
				
				flag = 0;
				rotary_detect_flag = 0;
				
				/* Reset the control token to inform upper layer that a transfer is ongoing */
				PrevXferComplete = 0;
  
				/* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
				USB_SIL_Write(EP1_IN, Send_Buffer, 8);
  
				/* Enable endpoint for transmission */
				SetEPTxValid(ENDP1);
      }
    }
  }
}

/*
 *���� ��ʼ��USB���ϵͳ��ʼ��
 *���� ��
 *���� ��
*/
void System_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	 /*����GPIO����*/
	/*ʹ�ܶ˿�A��ʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
							,ENABLE);
	/* Configure rotary pins*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*����Ϊ����������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /*����GPIO����*/
	/*ʹ�ܶ˿�B��ʱ��*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB
							,ENABLE);
	/* Configure KEY pins*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*����Ϊ����������*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*I2C���ų�ʼ��*/
	//I2C_GPIOInit();
	
	/*���ٶȴ������ĳ�ʼ��*/
	//ADXL345_Init();
	
	/*��ʱ���ĳ�ʼ��*/
	TIM3_Init();
}
/*
 *���� ����ֵ����
 *���� ��
 *���� ��
*/
void Key_Handler(void)
{
	float tempX,tempY,tempZ;
	//float roll,pitch,yaw;
	INT8S roll,pitch;

#if 0	
    
	//temp=(float)dis_data*3.9;  //�������ݺ���ʾ,�鿼ADXL345�������ŵ�4ҳ
  tempX = (float)ADXL345_data.ax * 0.0039;
  tempY = (float)ADXL345_data.ay * 0.0039;
  tempZ = (float)ADXL345_data.az * 0.0039;
	
	//roll =  (float)(((atan2(tempZ,tempX)*180)/3.1416)-90); //x��Ƕ�
	//pitch = (float)(((atan2(tempZ,tempY)*180)/3.1416)-90); //y��Ƕ�
	//yaw =   (float)((atan2(tempX,tempY)*180)/3.1416);      //Z��Ƕ�
  roll =  (INT8S)(((atan2(tempZ,tempX)*180)/3.1416)-90); //x��Ƕ�
	pitch = (INT8S)(((atan2(tempZ,tempY)*180)/3.1416)-90); //y��Ƕ�
	
	if(roll < -30)
		Send_Buffer[2] = 0x52; //Keyboard UpArrow
	else
		Send_Buffer[2] = 0x00;
	
	if(pitch > 30)
		Send_Buffer[3] = 0x50; //Keyboard LeftArrow
	else if(pitch < -30)
		Send_Buffer[3] = 0x4f; //Keyboard RightArrow
	else
		Send_Buffer[3] = 0x00;
	
	if(key_flag & 0x80) //���R���Ƿ���
		Send_Buffer[4] = 0x00;
	else
		Send_Buffer[4] = 0x15; 
	
	if(key_flag & 0x40) //���Shift�Ƿ���
		Send_Buffer[0] &= 0xfd;
	else
		Send_Buffer[0] |= 0x02;
	
	if((key_flag & 0x20) == 0x00) //����Ƿ�Ҫ�ɿ�Keyboard UpArrow
		Send_Buffer[2] = 0x00;
	
	/*if(key_flag & 0x10) //����Ƿ���Ctrl��
		Send_Buffer[0] &= 0xfe;
	else
		Send_Buffer[0] |= 0x01;*/
#else
	if((key_flag & 0x40) == 0x00) //���Keyboard PgUp�Ƿ���
		Send_Buffer[2] = 0x4B;
	else
		Send_Buffer[2] = 0x00; 
	
	if((key_flag & 0x01) == 0x00) //���Keyboard PgDn�Ƿ���
		Send_Buffer[3] = 0x4E;
	else
		Send_Buffer[3] = 0x00;
	
	/*if((key_flag & 0x20) == 0x00) //���Keyboard RightArrow�Ƿ���
		Send_Buffer[4] = 0x4f;
	else
		Send_Buffer[4] = 0x00;*/

	rotary_dir_flag[0] = rotary_dir[(rdata.rotary_curr & 0x0c) >> 2][(rdata.rotary_prev & 0x0c) >> 2];
	rotary_dir_flag[1] = rotary_dir[(rdata.rotary_curr & 0x30) >> 4][(rdata.rotary_prev & 0x30) >> 4];
	rotary_dir_flag[2] = rotary_dir[(rdata.rotary_curr & 0xc0) >> 6][(rdata.rotary_prev & 0xc0) >> 6];

	if (rotary_dir_flag[0] == ROTARY_DIR_F)
		Send_Buffer[4] = 0x4f;
	else if (rotary_dir_flag[0] == ROTARY_DIR_B)
		Send_Buffer[4] = 0x50;
	else
		Send_Buffer[4] = 0x00;

	if (rotary_dir_flag[1] == ROTARY_DIR_F)
		Send_Buffer[5] = 0x57;
	else if (rotary_dir_flag[1] == ROTARY_DIR_B)
		Send_Buffer[5] = 0x56;
	else
		Send_Buffer[5] = 0x00;

	if (rotary_dir_flag[2] == ROTARY_DIR_F)
		Send_Buffer[6] = 0x60;
	else if (rotary_dir_flag[2] == ROTARY_DIR_B)
		Send_Buffer[6] = 0x5A;
	else
		Send_Buffer[6] = 0x00;
#endif
}

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
