#ifndef __12864_h
#define __12864_h

#include "sys.h"

//本程序共两套完全独立的字体绘制

//字体类型
typedef struct {
	const uint8_t *(*getTab)(int ch);
	int heigth;
	int width;
}Font;

// 系统字体
#define	GDI_SYSFONT_WIDTH		  6		 //字体像素宽度
#define	GDI_SYSFONT_HEIGHT		9		 //字体像素高度
#define	GDI_SYSFONT_BYTEWIDTH	1		 //间隔一个像素
#define	GDI_SYSFONT_OFFSET		0x20 //ASCII起始值
#define	VID_VSIZE	  128

void Lcd_Init(void);
void LCD_Clear(void);
void LCD_Display_Picture(uint8_t *img);
void LCD_Display_Words(uint8_t x,uint8_t y,uint8_t*str);
void LCD_Display_Update(void);

void LCD_Draw_Point(uint8_t x,uint8_t y,uint8_t  color);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
void Gui_Clear(void);
void Gui_Circle(uint16_t X,uint16_t Y,uint16_t R,uint16_t fc) ;
void Gui_DrawLine(uint16_t x0, uint16_t y0,uint16_t x1, uint16_t y1,uint16_t Color);
void gui_draw_square(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t lineColor);

//font 8*16
void font_putChar(int x0, int y0, int ch,uint8_t col);
void font_putString(int x0, int y0, const char *str,uint8_t col);

//font 6*9
void gui_bitblt(u16 x,u16 y,u16 w,u16 h,u8 * ptx,uint8_t color);
void gui_draw_text(int16_t x, int16_t y, u8* ptext, uint8_t color);

#endif /*__12864_h*/
