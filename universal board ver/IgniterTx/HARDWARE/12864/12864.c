/**
  ******************************************************************************
  * @file    12864.c
  * @author  �Ŷ�
  * @version V1.0.0
  * @date    2020-1-3
  * @brief   12864��ʾ����������
  ******************************************************************************
  */
/*include file ---------------------------------------------------------------*/

#include "stm32f10x.h"
#include "12864.h"
#include "sys.h"
#include "delay.h"
#include "font.h"

#define WRITE_CMD	0xF8//д����  
#define WRITE_DAT	0xFA//д����

//     SID ������PE1
//     SCLK  ������PE0
#define SID 		PBout(12)
#define SCLK		PBout(13)

#define DELAY_BUSY 	 //delay_us(1)
#define DELAY_WRITE  delay_us(1)

uint8_t FrameBuffer[1024];//��ʾ������

static const Font *fontRoot = &font8x15;

void SendByte(u8 byte)
{
     u8 i; 
	  for(i = 0;i < 8;i++)
    {
        if((byte << i) & 0x80)  //0x80(1000 0000)  ֻ�ᱣ�����λ
		{
		    SID = 1;           // ��������ߵ�ƽ��������1
		}
		else
		{
			SID = 0;         // ��������͵�ƽ��������0
		}
		/*��		
		SID =	(Dbyte << i) & 0x80;
				
		��������Ϊ�˷������
		*/
		SCLK = 0;   //ʱ�����õ�  ����SID�仯
		DELAY_WRITE; //��ʱʹ����д��
		SCLK = 1;    //����ʱ�ӣ��ôӻ���SID
	}   
}
 

void Lcd_WriteCmd(u8 Cmd )
{
     DELAY_BUSY;    //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_CMD);            //11111,RW(0),RS(0),0   
     SendByte(0xf0&Cmd);      //����λ
     SendByte(Cmd<<4);   //����λ(��ִ��<<)
}
 

void Lcd_WriteData(u8 Dat )
{
     DELAY_BUSY;     //��������û��дLCD��æ�ļ�⣬����ֱ����ʱ1ms��ʹÿ��д�����ݻ�ָ��������1ms ��ɲ���дæ״̬���
     SendByte(WRITE_DAT);            //11111,RW(0),RS(1),0
     SendByte(0xf0&Dat);      //����λ
     SendByte(Dat<<4);   //����λ(��ִ��<<)
}


void lcd_GPIO_init()
{
	GPIO_InitTypeDef gpio;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	gpio.GPIO_Pin	 		= 	GPIO_Pin_12 | GPIO_Pin_13;
	gpio.GPIO_Mode 		= 	GPIO_Mode_Out_PP;
	gpio.GPIO_Speed  	= 	GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB,&gpio);
	
  SID=1;
  SCLK=1;
}


void Lcd_Init(void)
{ 
	lcd_GPIO_init();
	
	for(int i = 0;i < 1024;i++)
	{
			FrameBuffer[i] = 0x00;
	}
	
	
  delay_ms(50);   	//�ȴ�Һ���Լ죨��ʱ>40ms��
	Lcd_WriteCmd(0x30);        //�����趨:ѡ�����ָ�  ��ѡ��8bit������
  delay_ms(1);//��ʱ>137us 
  Lcd_WriteCmd(0x0c);        //����ʾ
  delay_ms(1);	//��ʱ>100us
  Lcd_WriteCmd(0x01);        //�����ʾ�������趨��ַָ��Ϊ00H
  delay_ms(30);	//��ʱ>10ms
	Lcd_WriteCmd(0x06);        //ÿ�ε�ַ�Զ�+1����ʼ�����
	
	Lcd_WriteCmd(0x34);		//�л�������ָ��
	LCD_Display_Update();
	Lcd_WriteCmd(0x36);//��ͼ����ʾ
}


void LCD_Clear(void)
{
	Lcd_WriteCmd(0x01);			//����ָ��
	delay_ms(2);				//��ʱ�Դ�Һ���ȶ�������1.6ms��
}


void LCD_Display_Picture(uint8_t *img)
{
	//Lcd_WriteCmd(0x34);		//�л�������ָ��
	//Lcd_WriteCmd(0x34);		//�ر�ͼ����ʾ
	for(int i=0;i<32;i++)      //�ϰ�����32��������д��
	{
			Lcd_WriteCmd(0x80+i); //���ʹ�ֱ��ַ
			Lcd_WriteCmd(0x80);   //����ˮƽ��ַ��ˮƽ��ַ���Լ�1
			for(int j=0;j<16;j++) //ÿ��128���㣬����һ��Ҫ16����λ16��������Ҳ����8λ��������������ȫ������
			{
					Lcd_WriteData(*img);
					img++;
			}
	}
	for(int i=0;i<32;i++)     //�°�����32�Ų���ԭ����ϰ���һ��
	{ 
			Lcd_WriteCmd(0x80+i);
			Lcd_WriteCmd(0x88);
			for(int j=0;j<16;j++)
			{
					Lcd_WriteData(*img);
					img++;
			}
	}
	//Lcd_WriteCmd(0x36);//��ͼ����ʾ
	//Lcd_WriteCmd(0x30);//�л��ػ���ָ��
}

void LCD_Display_Update(void)
{
	LCD_Display_Picture(FrameBuffer);
}

void LCD_Draw_Point(uint8_t x,uint8_t y,uint8_t  color)
{
	int pos = x / 8 + y * 16;
	
	if(x >= 128 || y >= 64)
	{
		return ;
	}
	
	switch (color)
	{
		case 0://����
		{
			FrameBuffer[pos] &= ~(0x80 >> x % 8);
			break;
		}
		case 1://��ʾ
		{
			FrameBuffer[pos] |= (0x80 >> x % 8);
			break;
		}
	}
}

/*GUI ---------------------------------------------------------------------*/

void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	LCD_Draw_Point(x,y,Data);
}  

void Gui_Clear(void)
{
	for(int i = 0;i < 1024;i++)
	{
		FrameBuffer[i] = 0;
	}
}

void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc) 
{//Bresenham�㷨 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//���ߺ�����ʹ��Bresenham �����㷨
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	

	dx = x1-x0;//����x����
	dy = y1-y0;//����y����

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)//x�������y���룬��ôÿ��x����ֻ��һ���㣬ÿ��y���������ɸ���
	{//���ߵĵ�������x���룬��x���������
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//Ҫ���ĵ������ᳬ��x����
		{
			//����
			Gui_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //�Ƿ���Ҫ����y����ֵ
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;//����y����ֵ
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x����ֵÿ�λ���󶼵���1
		} // end for
	} // end if |slope| <= 1
	else//y�����x�ᣬ��ÿ��y����ֻ��һ���㣬x�����ɸ���
	{//��y��Ϊ��������
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			Gui_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}

void gui_draw_square(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t lineColor)
{
	Gui_DrawLine(x0,y0,x0,y1,lineColor); 
	Gui_DrawLine(x0,y0,x1,y0,lineColor); 
	Gui_DrawLine(x1,y1,x0,y1,lineColor); 
	Gui_DrawLine(x1,y1,x1,y0,lineColor); 
}

//��ʾһ���ֽ�
void font_putChar(int x0, int y0, int ch,uint8_t col)
{
	if (fontRoot == 0) {
		return;
	}
	int byte = fontRoot->width / 8;
	const uint8_t *chMat = fontRoot->getTab(ch);
	for (int y = 0; y < fontRoot->heigth; y++) {
		for (int db = 0; db < byte; db++) {
			uint8_t val = *(chMat+db+(y*byte));
			for (int x = 0; x < 8; x++) {
				int color;
				int lsb = val & 0x01;
				val >>= 1;

				uint16_t xt = x0 + (x + (db*8));
				uint16_t yt = y0 + (y);

				if (lsb) {
					if(col == 1)
					Gui_DrawPoint(xt,yt,1);
					else
					Gui_DrawPoint(xt,yt,0);
				} else {
					if(col == 1)
					Gui_DrawPoint(xt,yt,0);
					else
					Gui_DrawPoint(xt,yt,1);
				}
				
			}
		}
	}
}

//��ʾһ���ַ���
void font_putString(int x0, int y0, const char *str,uint8_t col)
{
	int x = x0;
	int y = y0;
	for (int i = 0; str[i] != '\0'; i++) {
		switch (str[i]) {
		case '\b':
			x -= fontRoot->width;
			break;
		case '\t':
			for (int j = 0; j < 4; j++) {
				font_putChar(x, y, ' ',col);
				x += fontRoot->width;
			}
			break;
		case '\r':
			x = x0;
			break;
		case '\n':
			x = x0;
			y += fontRoot->heigth;
			break;
		default:
			font_putChar(x, y, str[i],col);
			x += fontRoot->width;
		}
	}
}

void gui_bitblt(u16 x,u16 y,u16 w,u16 h,u8 * ptx,uint8_t color)
{
	int num;
	for(int i  = 0;i < h;i++)
	{
		for(int j = 0;j < w;j++)
		{
			num = (w>8)?i*(w/8) + j/8:i + j/8;
			if((ptx[num]&(0x80>>(j & 0x07)))==0)
			{
				if(color == 1)
				Gui_DrawPoint(x + j,y + i,1);
				else
				Gui_DrawPoint(x + j,y + i,0);
			}
			else
			{
				if(color == 1)
				Gui_DrawPoint(x + j,y + i,0);
				else
				Gui_DrawPoint(x + j,y + i,1);
			}
		}
	}
}

void gui_draw_text(int16_t x, int16_t y, u8* ptext, uint8_t color)
{

	u16 i, pos, xp;
	u8	c;
	u8*	ptx;

	xp = x;
	for (i = 0;*ptext!='\0'; i++) {
		c = *(ptext++);
		if (c >= GDI_SYSFONT_OFFSET) {
			pos = (u16)(c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
			ptx = ((u8*) ascii_word) + pos;
			gui_bitblt(xp, y, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, color);
			xp += GDI_SYSFONT_WIDTH;
			if (xp >= VID_VSIZE) return;
		}
	}
}
