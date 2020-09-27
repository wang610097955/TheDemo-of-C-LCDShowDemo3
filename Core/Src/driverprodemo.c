#include "gpio.h"
#include "stdint.h"
#include "driverprodemo.h"
#include "gpio.h"
#include "lcdfont.h"


#define WRITE_CMD	0xF8
#define WRITE_DATA	0xFA
#define READ_DATA       0xFE

void LCD_RST();
void SendByte(uint8_t byte);
void LCD_DisplayFont2(uint8_t t);
void LCD_DisplayFont(uchar *CGRAM_ZIKU,uint8_t t);
static void set_Cursor(uchar x,uchar y);
static void disp12_CGRAM(uchar x, uchar y, uchar num);


//LCD Driver Demo
/********************************************************************************
*** Function  Name  ：void LCD_Init(void);
*** Function	    ：GPIO Initialization
*** Input  parameter：Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:Init the MCU GPIO Port and the LCD driver
********************************************************************************/
void LCD_Init(void)
{
  MX_GPIO_Init();

  LCD_RST(); 
  LCD_WriteCmd(0x30);         
  LCD_WriteCmd(0x0c);         
  LCD_WriteCmd(0x01);        
}

/********************************************************************************
*** Function  Name  ：void Delay_us(uint32_t us);
*** Function	    ：Delay_Time
*** Input  parameter：us:time
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void Delay_us(uint32_t us)
{
  while(us--);
}

/********************************************************************************
*** Function  Name  ：void Delay_ms(uint32_t ms);
*** Function        ：Delay_Time
*** Input  parameter：ms:time
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void Delay_ms(uint32_t ms)
{
  ms = 1000*ms;
  while(ms--);
}

/********************************************************************************
*** Function  Name   ：void LCD_WriteCmd(uint8_t Cmd);
*** Function        ：Control the commend  input
*** Input  parameter：Cmd:Command Input 8 bit
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_WriteCmd(uint8_t Cmd)
{ 
  LCD_CS_H;
//  LCD_CheckBusy();
  SendByte(WRITE_CMD);
  Delay_us(1);
  SendByte(0xf0&Cmd);
  Delay_us(1);
  SendByte((Cmd<<4)&0xf0);
  LCD_CS_H;
}

/********************************************************************************
*** Function  Name  ：void LCD_WriteDat(uint8_t data);
*** Function        ：Control the data  input
*** Input  parameter：Dat:Data Input 8 bit
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_WriteDat(uint8_t Dat)
{
  LCD_CS_H;
//  LCD_CheckBusy();
  SendByte(WRITE_DATA); 
  Delay_us(1);
  SendByte(0xf0&Dat);
  Delay_us(1);
  SendByte((Dat<<4)&0xf0);
  LCD_CS_H;
}

/********************************************************************************
*** Function  Name  ：uchar LCD_ReadData();
*** Function        : Read  Data 
*** Input  parameter: Null
*** Ouput  parameter：Null
*** Return parameter：LCD_Data:the return LCD_Data
*					  LCD_Status:the registe Status
***
*** description:
********************************************************************************/
uint8_t LCD_ReadData()
{
  uint8_t i,temp_H,temp_L;
  LCD_WriteCmd(READ_DATA);
  LCD_CS_H;
  LCD_CLK_L;
  for(i=0;i<8;i++)
  {
    temp_H<<1;
    LCD_CLK_H;
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==GPIO_PIN_SET)
    {
      temp_H |=0x01;
    }
    else
    {
      temp_H |=0x00;
    }
    LCD_CLK_L;
  }
  temp_H &= 0xF0;
  for(i=0;i<8;i++)
  {
    temp_L<<1;
    LCD_CLK_H;
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)==GPIO_PIN_SET)
    {
      temp_L &= 0x01;
    }
    else
    {
      temp_L &= 0x00;
    }
    LCD_CLK_L;      
  }
  temp_L &=0xF0;
  temp_H = temp_H|(temp_L>>4);
  LCD_CS_L;
  return temp_H;
}

/********************************************************************************
*** Function  Name  ：void LCD_CheckBusy(void)
*** Function	    : Read the System Buffer status
*** Input  parameter: Null
*** Ouput  parameter：Null
*** Return parameter: LCD_Status:the Buffer status
*				
***
*** description:
********************************************************************************/
void LCD_CheckBusy(void)
{
  do
  {
    LCD_WriteCmd(0xFC);
  }while(0x80&&LCD_ReadData);
}

/********************************************************************************
*** Function  Name      ：void LCD_Display_Nums(uint8_t x, uint8_t y, uint8_t num);
*** Function	       ：LCD display Numbers
*** Input  parameter: x:the Abscissa
*		      y:the Ordinate
*		    num:display number
*** Ouput  parameter：LCD display number
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_Display_Nums(uint8_t x, uint8_t y, uint8_t num)
{
  uint8_t i = num+0x30;
  LCD_WriteCmd(LCD_addr[x][y]); 
  LCD_WriteDat(i);
}

/********************************************************************************
*** Function  Name	：void LCD_Display_String(uint8_t x, uint8_t y, uint8_t* str);
*** Function	        ：LCD display String
*** Input  parameter: x:the Abscissa
*		      y:the Ordinate
*		    str:display the String what you want
*** Ouput  parameter：LCD display string
*** Return parameter：Null
***
*** description:when you use the function,you need to add LCD_Clear() before.
                
********************************************************************************/
void LCD_Display_String(uint8_t x, uint8_t y, uint8_t* str)
{ 
  set_Cursor(x,y); 
  while(*str>0)
  { 
    LCD_WriteDat(*str);    
    str++;     
  }
}

/********************************************************************************
*** Function  Name	：void LCD_Clear(void)
*** Function		：Clear the LCD display
*** Input  parameter: Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_Clear(void)
{
  LCD_WriteCmd(0x01);
  HAL_Delay(10);
}

/********************************************************************************
*** Function  Name	：void LCD_BLK_Open(void)
*** Function		：Open the behind light
*** Input  parameter: Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_BLK_Open(void)
{
  Delay_us(1);
  HAL_GPIO_WritePin(BLK_Port,BLK_Pin,GPIO_PIN_SET);
}

/********************************************************************************
*** Function  Name	：void LCD_BLK_Close(void)
*** Function		：Close the behind light
*** Input  parameter: Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_BLK_Close(void)
{
  Delay_us(1);
  HAL_GPIO_WritePin(BLK_Port,BLK_Pin,GPIO_PIN_RESET);
}

/********************************************************************************
*** Function  Name	：static void disp12_CGRAM(uchar x,uchar y,uchar num)
*** Function		：display the 12-fontsize
*** Input  parameter：x:the Abscissa
*		      y:the Ordinate
*                   num:the font number in lcdfont.h
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
static void disp12_CGRAM(uchar x,uchar y,uchar num)
{       
  set_Cursor(x,y);
  LCD_WriteDat(0x00);
  LCD_WriteDat(num*2);
}

/********************************************************************************
*** Function  Name	：void LCD_Display_Imag(const uchar* ptr);
*** Function		：LCD display Images
*** Input  parameter: ptr:the pictures parameters
*** Ouput  parameter：LCD display image
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_Display_Imag(const uint8_t site)
{
  uint8_t* ptr;
	unsigned int i,k;
  switch(site)
  {
  case 0:ptr = logo;break;
  default:break;
  }

  LCD_WriteCmd(0x34);
  for(i=0;i<32;i++)
  {
    LCD_WriteCmd(0x80+i);
    LCD_WriteCmd(0x80);
    for(k=0;k<16;k++)
    {
        LCD_WriteDat(*ptr);
        ptr++;
    }
  }
  for(i=0;i<32;i++)
  {
    LCD_WriteCmd(0x80+i);
    LCD_WriteCmd(0x88);
    for(k=0;k<16;k++)
    {
        LCD_WriteDat(*ptr);
        ptr++;
    }
  }
  LCD_WriteCmd(0x36); 
  LCD_WriteCmd(0x30); 
}



/********************************************************************************
*** Function  Name	：void LCD_DisplayFont12(uchar x,uchar y,uint8_t t)
*** Function		：Display the Chinese Fontzise of 12
*** Input  parameter: x:the Abscissa
		      y:the Ordinate
                      t:the Chinese font in lcdfont place
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:but the function only to make 4 fonts,and you need to make up
                the 32 hexs in the last,add 0x00
********************************************************************************/
void LCD_DisplayFont12(uchar x,uchar y,uint8_t t)
{
  uchar *CGRAM_ZIKU;
  switch(t)
  {
  case 0:CGRAM_ZIKU = CGRAM_ZIKU12;break;
  case 1:CGRAM_ZIKU = CGRAM_ZIKU13;break;
  case 2:CGRAM_ZIKU = CGRAM_ZIKU14;break;
  case 3:CGRAM_ZIKU = CGRAM_ZIKU15;break;
  }
  LCD_DisplayFont(CGRAM_ZIKU,t);
  disp12_CGRAM(x,y,t);
}

/********************************************************************************
*** Function  Name	：static void LCD_DisplayFont(uchar *CGRAM_ZIKU,uint8_t t)
*** Function		：input the 12-size or others font 
*** Input  parameter：*CGRAM_ZIKU:the font what you want to input
                                t:the font's number
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:you can revise CGRAM_ZIKU1X in lcdfont.h via matrix software
********************************************************************************/
static void LCD_DisplayFont(uchar *CGRAM_ZIKU,uint8_t t)
{
  uchar i,add;
  LCD_WriteCmd(0x34);                //8 bit and expand command open
  LCD_WriteCmd(0x02);                //enable the CGRAM site
  LCD_WriteCmd(0x30);                //8 bit and basic command open   
  add=(0x40+(t<<4));                 //count the CGRAM site 
  for(i=0;i<16;i++)
  {
    LCD_WriteCmd(add+i);             //set the first site of CGRAM
    LCD_WriteDat(CGRAM_ZIKU[i*2]);   //input the high 8 bit site
    LCD_WriteDat(CGRAM_ZIKU[i*2+1]); //input the low  8 bit site
  }

}

/********************************************************************************
*** Function  Name	：void LCD_Init(void);
*** Function		：GPIO Initialization
*** Input  parameter：Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void LCD_DrawPoiot(uchar dat)
{
   uchar i,j,k;
   uchar GDRAM_X=0X80;
   uchar GDRAM_Y=0x80;
   for(i=0;i<2;i++)
   {
     for(j=0;j<32;j++)
     {
       for(k=0;k<8;k++)
       {
         LCD_WriteCmd(0x34);
         LCD_WriteCmd(GDRAM_X+k);
         LCD_WriteCmd(GDRAM_Y+j);
         LCD_WriteDat(dat);
         LCD_WriteDat(dat);
       }
     }
     GDRAM_X = 0x88;
   }
   LCD_WriteCmd(0x36);
   LCD_WriteCmd(0x30);  
}

/********************************************************************************
*** Function  Name	：void set_Cursor(uchar x,uchar y)
*** Function		：GPIO Initialization
*** Input  parameter：x:the Abscissa
*		              y:the Ordinate
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:make sure the input of location
********************************************************************************/
static void set_Cursor(uchar x,uchar y)
{
  uchar i;
  switch(x)
  {
  case 0x00:i = 0x80;break;
  case 0x01:i = 0x90;break;
  case 0x02:i = 0x88;break;
  case 0x03:i = 0x98;break;
  default:break;
  }
  i = y+i;
  LCD_WriteCmd(i);
}

/********************************************************************************
*** Function  Name	：void Fill_GDRAM(uchar dat)
*** Function		：GPIO Initialization
*** Input  parameter：Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void Fill_GDRAM(uchar dat)
{
  uchar i,j,k;
  uchar GDRAM_X =0x80;
  uchar GDRAM_Y =0x80;
  for(i=0;i<2;i++)
  {
    for(j=0;j<32;j++)
    {
      for(k=0;k<8;k++)
      {
        LCD_WriteCmd(0x34);
        LCD_WriteCmd(GDRAM_Y+j);
        LCD_WriteCmd(GDRAM_X+k);
        LCD_WriteDat(dat);
        LCD_WriteDat(dat);
      }
   }
    GDRAM_X = 0x88;
 }
  LCD_WriteCmd(0x36);
  LCD_WriteCmd(0x30);
}

/********************************************************************************
*** Function  Name	：void Fill_Point(uint8_t x,uint8_t y,uchar color)
*** Function		：GPIO Initialization
*** Input  parameter：Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
void Fill_Point(uint8_t x,uint8_t y,uchar color)
{
  uint8_t x_Dyte,x_dyte;
  uint8_t y_Dyte,y_dyte;
  uchar GDRAM_H,GDRAM_L;
  LCD_WriteCmd(0x36);
  
  x_Dyte = x/16;
  x_dyte = x%0x0f;
  y_Dyte = y/32;
  y_Dyte = y%0x1f;
  
  LCD_WriteCmd(0x80+y_dyte);
  LCD_WriteCmd(0x80+x_Dyte+8*y_Dyte);
  LCD_ReadData();
  GDRAM_H = LCD_ReadData();
  GDRAM_L = LCD_ReadData();
  HAL_Delay(1);

  LCD_WriteCmd(0x80+y_dyte);
  LCD_WriteCmd(0x80+x_Dyte+8*y_Dyte);  
  HAL_Delay(10);
  
  if(x_dyte<8)
  {
    if(color == 1)
    {
      LCD_WriteDat(GDRAM_H|(0x01<<(7-x_dyte)));       
    }
    else
    {
      LCD_WriteDat(GDRAM_H&(~(0x01<<(7-x_dyte))));
    }
      LCD_WriteDat(GDRAM_L);
  }
  else
  {
      LCD_WriteDat(GDRAM_H);
    if(color == 1)
    {
      LCD_WriteDat(GDRAM_L|(0x01<<(15-x_dyte)));
      LCD_WriteDat(GDRAM_L&(~(0x01<<(15-x_dyte))));
    }
    LCD_WriteCmd(0x30);
  }
  
}

/********************************************************************************
*** Function  Name	：static void LCD_RST()
*** Function		：LCD Reset control
*** Input  parameter：Null
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:control the Reset GPIO
********************************************************************************/
static void LCD_RST()
{
  LCD_RST_L;
  HAL_Delay(5);
  LCD_RST_H;
  HAL_Delay(5);
}

/********************************************************************************
*** Function  Name	:static void SendByte(uint8_t byte)
*** Function		:MCU control send the byte to control LCD
*** Input  parameter：byte : the MCU send byte control the GPIO
*** Ouput  parameter：Null
*** Return parameter：Null
***
*** description:
********************************************************************************/
static void SendByte(uint8_t byte)
{  
  uint8_t i;
  for (i = 0; i < 8; i++)
  {
    if ((byte<<i)&0x80)
    {
      LCD_SID_H;
    }
    else
    {
      LCD_SID_L;
    }
    LCD_CLK_H;
    Delay_us(1);
    LCD_CLK_L;
    Delay_us(1);
  }
}

void init_picture()
{
//  LCD_WriteCmd(0x34);
//  LCD_WriteCmd(0x36);
    LCD_WriteCmd(0x3E);
    LCD_WriteCmd(0x01);
}

void show_Pic(uchar*  address)
{
  uchar i,j;
  for(i=0;i<32;i++)
  {
    LCD_WriteCmd(0x80+i);
    LCD_WriteCmd(0x80);
    for(j=0;j<16;j++)
    {
      LCD_WriteDat(*address);
      address++;
    }
  }
  for(i=0;i<32;i++)
  {
    LCD_WriteCmd(0x80+i);
    LCD_WriteCmd(0x88);
    for(j=0;j<16;j++)
    {
      LCD_WriteDat(*address);
      address++;
    }
  }
}

void Gui_Line(uchar x0,uchar x1,uchar y0,uchar color)
{
  uchar temp;
  if(x0>x1)
  {
    temp=x0;
    x0 = x1;
    x1 = temp;
  }
  do
  {
    Fill_Point(x0,y0,color);
    x0++;
    
  }while(x1>=x0);
}

void LCD_Test_Init()
{
  MX_GPIO_Init();
  LCD_RST(); 
  LCD_WriteCmd(0x30);
  LCD_WriteCmd(0x30);
  LCD_WriteCmd(0x08);
  LCD_WriteCmd(0x10);
  LCD_WriteCmd(0x0C);
  LCD_WriteCmd(0x01);
  LCD_WriteCmd(0x06);
}

uint8_t huancun[32][16][2];
void DrawDots(uint8_t x,uint8_t y,uint8_t color)
{
  uint8_t ylabel,xlabel,xlabel_bit;
  uint8_t Read_H = 0,Read_L = 0;
  LCD_WriteCmd(0x34);
  
  xlabel=x>>4;
  xlabel_bit = x&0x0f;
  if(y<32)
    ylabel = y;
  else
  {
    ylabel = y-32;
    xlabel+=8;
  }
  LCD_WriteCmd(ylabel+0x80);
  LCD_WriteCmd(xlabel+0x80);
  Read_H = huancun[ylabel][xlabel][1];
  Read_L = huancun[ylabel][xlabel][2];
  
  if(xlabel_bit<8)
  {
      switch(color)
       {
         case 0:Read_H&=(~(0x01<<(7-xlabel_bit)));huancun[ylabel][xlabel][1]=Read_H; break; 
         case 1:Read_H|=(0x01<<(7-xlabel_bit));huancun[ylabel][xlabel][1]=Read_H;break; 
         case 2:Read_H^=(0x01<<(7-xlabel_bit));huancun[ylabel][xlabel][1]=Read_H;break; 
         default:break;
       }
      LCD_WriteDat(Read_H);
      LCD_WriteDat(Read_L);
  }
  else
  {
    switch(color)
       {
         case 0:Read_L&=(~(0x01<<(15-xlabel_bit)));huancun[ylabel][xlabel][2]=Read_L; break;
         case 1:Read_L|=(0x01<<(15-xlabel_bit));huancun[ylabel][xlabel][2]=Read_L;break; 
         case 2:Read_L^=(0x01<<(15-xlabel_bit));huancun[ylabel][xlabel][2]=Read_L;break; 
         default:break;
       }
      LCD_WriteDat(Read_H);
      LCD_WriteDat(Read_L);
  }
     LCD_WriteCmd(0x36);      
     LCD_WriteCmd(0x30);
  
}






