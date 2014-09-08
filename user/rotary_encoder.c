/****************************************************************************
* �ļ�����rotary_encoder.c
* ���ݼ���: 
*			      ��ת��������غ���
*			   
*			                          
*�ļ���ʷ��
*			�汾��	  ��������		����
*			 v0.1	 2014/09/08	   LuoHui
* ˵    ����
****************************************************************************/

#include "include.h"
#include "rotary_encoder.h"
#include <stdio.h>
#include <math.h>

/* ��ť��������־��Ϣ */
INT8U rotary_flag = 0xfc;
INT8U rotary_start_flag = 0;
INT8U rotary_detect_flag = 0;

struct rotary_data rdata = {0};


INT8U rotary_dir_flag[3] = {ROTARY_DIR_NONE, ROTARY_DIR_NONE, ROTARY_DIR_NONE};

/* rotary button variables */
INT32U rotary_count = 0;
__IO INT8U rotary1 = 0x00;
__IO INT8U rotary2 = 0x00;


/* ��ת������AB�����
**�б�ʾ��ǰAB��λ��ƽ��
**�б�ʾ�ϴ�AB��λ��ƽ */

INT8S rotary_dir[4][4] = {
	      /* 00   01   10   11 */
	/* 00 */  0,  1,  2, -1,
	/* 01 */  2,  0, -1,  1,
	/* 10 */  1, -1,  0,  2,
	/* 11 */ -1,  2,  1,  0

};


void detect_rotary(void)
{
	/*���浱ǰ�����˿�״̬*/
//  static	__IO INT8U key1 = 0xf0;
	/*������һ״̬�����˿�״̬*/
//	static	__IO INT8U key2 = 0xf0;
	
	INT16U rotary = 0;

	/* ������ȡ��ť�������˿� */
	rotary = GPIO_ReadInputData(GPIOA);

	rotary1= (INT8U)rotary;
	
	rotary1 &= 0xfc;

	if (rotary_start_flag)
	{
		/*��ʾ��ť�����������˸ı�*/
		if(rotary1 != rdata.rotary_curr)
		{
			rotary_count++; //�൱����ʱ
			if(rotary_count >= 0x03)
			{
				//rotary_dir_flag[0] = rotary_dir[(rotary1 & 0x0c) >> 2][(rotary2 & 0x0c) >> 2];
				//rotary_dir_flag[1] = rotary_dir[(rotary1 & 0x30) >> 4][(rotary2 & 0x30) >> 4];
				//rotary_dir_flag[2] = rotary_dir[(rotary1 & 0xc0) >> 6][(rotary2 & 0xc0) >> 6];
				//rotary2 = rotary1;
				//rotary_flag = rotary2;
				rdata.rotary_prev = rdata.rotary_curr;
				rdata.rotary_curr = rotary1;
				rotary_count = 0;
				//rotary_flag = rotary1;
				rotary_detect_flag = 1;
			}
		}
		else
		{
			rotary_count++; //�൱����ʱ
			if(rotary_count >= 0x03)
			{
				//rotary_dir_flag[0] = ROTARY_DIR_NONE;
				//rotary_dir_flag[1] = ROTARY_DIR_NONE;
				//rotary_dir_flag[2] = ROTARY_DIR_NONE;
				//rotary_dir_flag[0] = rotary_dir[(rotary1 & 0x0c) >> 2][(rotary2 & 0x0c) >> 2];
				//rotary_dir_flag[1] = rotary_dir[(rotary1 & 0x30) >> 4][(rotary2 & 0x30) >> 4];
				//rotary_dir_flag[2] = rotary_dir[(rotary1 & 0xc0) >> 6][(rotary2 & 0xc0) >> 6];
				rdata.rotary_prev = rdata.rotary_curr;
				rdata.rotary_curr = rotary1;
				rotary_count = 0;
				rotary_detect_flag = 1;
			}
		}
		
		/*���ñ�־*/
		//rotary_detect_flag = 1;
	}
	else
	{
		rotary2 = rotary1;
		rdata.rotary_curr = rdata.rotary_prev = rotary1;
		rotary_start_flag = 1;
	}

}


