/*
******************************************************************************
* HX8352 Driver Core
* Branch Version : 1.3.0
* Ramin Jafarpour @ ECA.ir Co.			10/25/15
* raminmicro@gmail.com
* www.ECA.ir
* www.Eshop.ECA.ir
* www.Forum.ECA.ir
******************************************************************************
*/
#ifndef _LCD_H
#define _LCD_H

#include "lpc17xx.h"

typedef unsigned          char u8;
typedef unsigned short     int u16;
typedef unsigned           int u32;
typedef unsigned       __int64 u64;

#define ORIENTATION_VERTICAL

#define DATA_LSB_GPIO 	LPC_GPIO0
#define LCD_LSB_OFFSET  16

#define DATA_MSB_GPIO 	LPC_GPIO2
#define LCD_MSB_OFFSET 	0

#define LCD_CS_GPIO 	LPC_GPIO1
#define PIN_CS    		(1UL << 28)			

#define LCD_RS_GPIO 	LPC_GPIO0
#define PIN_RS    		(1UL << 25)	
			
#define LCD_WR_GPIO 	LPC_GPIO2
#define PIN_WR    		(1UL << 9)	

#define LCD_RD_GPIO 	LPC_GPIO2
#define PIN_RD    		(1UL << 13)	

#define LCD_CS(x)     	((x) ? (LCD_CS_GPIO->FIOSET = PIN_CS) : (LCD_CS_GPIO->FIOCLR = PIN_CS));
#define LCD_RS(x)     	((x) ? (LCD_RS_GPIO->FIOSET = PIN_RS) : (LCD_RS_GPIO->FIOCLR = PIN_RS));
#define LCD_WR(x)     	((x) ? (LCD_WR_GPIO->FIOSET = PIN_WR) : (LCD_WR_GPIO->FIOCLR = PIN_WR));
#define LCD_RD(x)     	((x) ? (LCD_RD_GPIO->FIOSET = PIN_RD) : (LCD_RD_GPIO->FIOCLR = PIN_RD));
#define LCD_RST(x)    	
                            
/* LCD RGB color definitions */
#define Black           0x0000		/*   0,   0,   0 */
#define Navy            0x000F      /*   0,   0, 128 */
#define DarkGreen       0x03E0      /*   0, 128,   0 */
#define DarkCyan        0x03EF      /*   0, 128, 128 */
#define Maroon          0x7800      /* 128,   0,   0 */
#define Purple          0x780F      /* 128,   0, 128 */
#define Olive           0x7BE0      /* 128, 128,   0 */
#define LightGrey       0xC618      /* 192, 192, 192 */
#define DarkGrey        0x7BEF      /* 128, 128, 128 */
#define Blue            0x001F      /*   0,   0, 255 */
#define Green           0x07E0      /*   0, 255,   0 */
#define Cyan            0x07FF      /*   0, 255, 255 */
#define Red             0xF800      /* 255,   0,   0 */
#define Magenta         0xF81F      /* 255,   0, 255 */
#define Yellow          0xFFE0      /* 255, 255, 0   */
#define White           0xFFFF      /* 255, 255, 255 */
#define WHITE           0XFFFF      /* 255, 255, 255 */   
#define RED             0xF800      /* 255,   0,   0 */
#define BLUE            0x001F      /*   0,   0, 255 */


#define R32            0x20
#define R33            0x21
#define R34            0x22
#define R80            0x50
#define R81            0x51
#define R82            0x52
#define R83            0x53


#ifdef ORIENTATION_VERTICAL

	#define WIDTH       	240 
	#define HEIGHT      	400

#else

	#define WIDTH       	400
	#define HEIGHT      	240

#endif


void LCD_Init           (void);
void LCD_Window			(u16 Xstart, u16 Ystart, u16 Xend, u16 Yend);
void LCD_WindowMax      (void);
void LCD_PutPixel 		(u16 x, u16 y, u16 color); 
void LCD_Clear          (u16 color);
void LCD_Fill			(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color);
void LCD_SetTextColor   (u16 color);
void LCD_SetBackColor   (u16 color);
void LCD_Show_Image		(int x0, int y0, int w, int h, const u16 *code);			
void LCD_ShowChar		(u16 x, u16 y, u8 num, u8 size, u8 mode);
void LCD_ShowString		(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);
void LCD_ShowxNum		(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);
void LCD_DrawLine		(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawRectangle	(u16 x1, u16 y1, u16 x2, u16 y2);
void Draw_Circle		(u16 x0,u16 y0,u8 r);

#endif

