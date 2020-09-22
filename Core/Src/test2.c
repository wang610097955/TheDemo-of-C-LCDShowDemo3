#include "gpio.h"
#include "stdint.h"
#include "driverprodemo.h"
void write_LCD1602_DB(uint8_t data)
{
	GPIO_Write(GPIOG,(uint16_t)data);
}
 
uint8_t read_LCD1602_DB(void)
{
	uint8_t value;
	value=(uint8_t)GPIO_ReadInputData(GPIOG);
	return value;
}
 
/* �ȴ�Һ��׼���� */
void LcdWaitReady()
{
    unsigned char sta;
		delay_us(1);
 
    LCD1602_RS = 0;
		delay_us(1);
 
    LCD1602_RW = 1;
		delay_us(1);
 
    do {
        LCD1602_E = 1;
		delay_us(1);
 
		LCD1602_DB_IO_inputMode();
		sta = read_LCD1602_DB();
		LCD1602_DB_IO_outputMode();	
		delay_us(1);
 
        LCD1602_E = 0;
		delay_us(1);
 
    } while (sta & 0x80); 	//bit7����1��ʾҺ����æ���ظ����ֱ�������0Ϊֹ
}
/* ��LCD1602Һ��д��һ�ֽ����cmd-��д������ֵ */
void LcdWriteCmd(unsigned char cmd)
{
		delay_us(1);
 
    LcdWaitReady();
		delay_us(1);
 
    LCD1602_RS = 0;
		delay_us(1);
 
    LCD1602_RW = 0;
		delay_us(1);
 
    write_LCD1602_DB(cmd);
		delay_us(1);
 
    LCD1602_E  = 1;
		delay_us(1);
 
    LCD1602_E  = 0;
		delay_us(1);
 
}
 
/* ��LCD1602Һ��д��һ�ֽ����ݣ�dat-��д������ֵ */
void LcdWriteDat(unsigned char dat)
{
		delay_us(1);
 
    LcdWaitReady();
		delay_us(1);
 
    LCD1602_RS = 1;
		delay_us(1);
 
    LCD1602_RW = 0;
		delay_us(1);
 
    write_LCD1602_DB(dat);
		delay_us(1);
 
    LCD1602_E  = 1;
		delay_us(1);
 
    LCD1602_E  = 0;
		delay_us(1);
 
}
/* ������ʾRAM��ʼ��ַ���༴���λ�ã�(x,y)-��Ӧ��Ļ�ϵ��ַ����� */
void LcdSetCursor(unsigned char x, unsigned char y)
{
    unsigned char addr;
    
	if (y >= 2)	   						//˵����������ʾ���Ұ���
    {
        y -= 2;
        x += 8;							//��ôx�͵�����Ų��8��������
    }
    addr = y*16 + x;					//�����������һ������Ϊ��λ������������˵�ĺ���Ϊ��λ����˼��16*16
										//����ʼDDRAM��ַ����д���ַ���				  
										//addr��ֵ�����еĺ��ֺ���ĵ�ַ��Ҳ�Ǻ���Ϊ��λ����ddramҲ���Ժ���Ϊ��λ��
    LcdWriteCmd(0x30); 					//����DDRAM����
    LcdWriteCmd(0x80|addr);				//����ram��ַ
}
/* ��Һ������ʾ�ַ�����(x,y)-��Ӧ��Ļ�ϵ���ʼ���꣬str-�ַ���ָ�� */
void LcdShowStr(unsigned char row, unsigned char column, unsigned char *str)
{
	unsigned char n=0,a;
	unsigned char *fore_str;
    LcdSetCursor(column, row);   		//������ʼ��ַ
    while (*str != '\0')  				//����д���ַ������ݣ�ֱ����⵽������
    {
		
		if((n+1)%2==0)					//������ַ
		{
			fore_str=str-1;
			if(*fore_str>0x80)
			{
				LcdWriteDat(*str++);
			}
			else if(*str>0x80)
				LcdWriteDat(' ');
			else if(*str<=0x80)
				LcdWriteDat(*str++);
		}			
		else
			LcdWriteDat(*str++);  		//��ȡstrָ������ݣ�Ȼ��str�Լ�1
		n++;
 
		if((n+2*column)%16==0)
		{
			a=(n+2*column)/16;
			if(row==0)
			{
			if(a==1)
				LcdWriteCmd(0x90);
			if(a==2)
				LcdWriteCmd(0x88);
			if(a==3)
				LcdWriteCmd(0x98);
			if(a==4)
				LcdWriteCmd(0x80);
			}
 
			if(row==1)
			{
			if(a==1)
				LcdWriteCmd(0x88);
			if(a==2)
				LcdWriteCmd(0x98);
			if(a==3)
				LcdWriteCmd(0x80);
			if(a==4)
				LcdWriteCmd(0x90);
			}	
 
			if(row==2)
			{
			if(a==1)
				LcdWriteCmd(0x98);
			if(a==2)
				LcdWriteCmd(0x80);
			if(a==3)
				LcdWriteCmd(0x90);
			if(a==4)
				LcdWriteCmd(0x88);
			}	
 
			if(row==3)
			{
			if(a==1)
				LcdWriteCmd(0x80);
			if(a==2)
				LcdWriteCmd(0x90);
			if(a==3)
				LcdWriteCmd(0x88);
			if(a==4)
				LcdWriteCmd(0x98);
			}	
 
 
	
		}
    }
}/* ��ʼ��1602Һ�� */
void InitLcd1602()
{
    LcdWriteCmd(0x38);  //16*2��ʾ��5*7����8λ���ݽӿ�
    LcdWriteCmd(0x0C);  //��ʾ���������ر�
    LcdWriteCmd(0x06);  //���ֲ�������ַ�Զ�+1
    LcdWriteCmd(0x01);  //����
}
 
 
 
 
/*
* ��������LcdShowImage
* ��  ��������ʾ������ʾһ��ͼ��
* ��  �룺img - ����ʾͼ��ָ��
*         x - ��Ļ��ʾ������(������Ϊ��λ)
*         y - ��Ļ��ʾ������(������Ϊ��λ)
*         w - ͼ����(������Ϊ��λ)
*         h - ͼ��߶�(������Ϊ��λ)
* ��  ������
* ��  ע��x��w������16������������CGRAM��СѰַ��λΪ2�ֽڣ�y��h��Ϊ0-63������ֵ��
*/
 
//�����ҵĲ��ϲ��ԣ�y��h��ֵҲ�ǿ���ȡ��64��������ʾ���֣�Ҳ��ȡ��16������15����ô��˵���Ƕ������ظ߶ȣ�����������
void LcdShowImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *img)
{
    uint16_t i;
    uint8_t xi,yi;
    uint8_t xt,yt;
 
    x >>= 4;
    w >>= 3;
    i = 0;
    LcdWriteCmd(0x36); //����CGRAM����
	
    for (yi=0; yi<h; yi++)
    {
        yt = y+yi;
        xt = x;
        if (yt >= 32)
        {
            yt -= 32;
            xt += 8;
        }
        LcdWriteCmd(0x80|yt);
        LcdWriteCmd(0x80|xt);
        for (xi=0; xi<w; xi++)
        {
            LcdWriteDat(img[i++]);
        }
    }
}
 
/*
* ��������LcdClearArea
* ��  ���������Ļ�ϵ�һ��ͼ����ʾ����
* ��  �룺x - ������ʼ������(������Ϊ��λ)
*         y - ������ʼ������(������Ϊ��λ)
*         w - ������(������Ϊ��λ)
*         h - ����߶�(������Ϊ��λ)
* ��  ������
* ��  ע��x��w������16������������CGRAM��СѰַ��λΪ2�ֽڣ�y��h��Ϊ0-63������ֵ��
*/
 
 
//��ס����ͼģʽʱ����ĻĬ�����ҵ���ʾ�����ݵģ���Ҫ�����ȵ�����������������������ʾ
 
void LcdClearArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    uint8_t xi,yi;
    uint8_t xt,yt;
 
    x >>= 4;
    w >>= 3;
    LcdWriteCmd(0x36); 			//����CGRAM����
    for (yi=0; yi<h; yi++)
    {
        yt = y+yi;
        xt = x;
        if (yt >= 32)
        {
            yt -= 32;
            xt += 8;
        }
        LcdWriteCmd(0x80|yt);
        LcdWriteCmd(0x80|xt);
        for (xi=0; xi<w; xi++)
        {
            LcdWriteDat(0x00); 	//д��0x00�������ʾ
        }
    }
}
