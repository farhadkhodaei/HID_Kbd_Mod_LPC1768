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

#include "HX8352.h"
#include "Font_24x16.h"
#include "stdio.h"


static volatile unsigned short TextColor = Black, BackColor = White;
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF; 
uint16_t DeviceCode;

__asm void wait()
{
    nop
    BX lr
}

void wait_delay(int count)
{
  while(count--);
}

void delayms(int dly)
{
	int i;
	while(dly--)
	{
		for(i=0;i<10000;i++)
		wait();
	}
}

/*******************************************************************************
* Send 1 byte over serial communication                                        *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:                                                                    *
*******************************************************************************/

static __inline void lcd_send (unsigned short byte) 
{
	DATA_MSB_GPIO->FIOCLR = (0xFF<<LCD_MSB_OFFSET);
	DATA_MSB_GPIO->FIOSET = ((byte>>8)<<LCD_MSB_OFFSET);
	wait();
	DATA_LSB_GPIO->FIOCLR = (0xFF<<LCD_LSB_OFFSET);
	DATA_LSB_GPIO->FIOSET = ((byte&0xFF)<<LCD_LSB_OFFSET);
	
	wait();

}


/*******************************************************************************
* read 1 byte over serial communication                                        *
*   Parameter:    byte:   byte to be sent                                      *
*   Return:                                                                    *
*******************************************************************************/

static __inline unsigned short lcd_read (void) 
{
	uint32_t data;

	DATA_MSB_GPIO->FIODIR &= (~(0xFF<<LCD_MSB_OFFSET));  
	DATA_LSB_GPIO->FIODIR &= (~(0xFF<<LCD_LSB_OFFSET));
	
	wait_delay(5);	
	data = (DATA_MSB_GPIO->FIOPIN >>LCD_MSB_OFFSET) & 0xFF;
	data <<= 8;
	data |= (DATA_LSB_GPIO->FIOPIN >>LCD_LSB_OFFSET) & 0xFF;

	DATA_MSB_GPIO->FIODIR |= (0xff<<LCD_MSB_OFFSET);  
	DATA_LSB_GPIO->FIODIR |= (0xff<<LCD_LSB_OFFSET); 
	wait();
	
	return(data); 
}

/*******************************************************************************
* Write command to LCD controller                                              *
*   Parameter:    c:      command to be written                                *
*   Return:                                                                    *
*******************************************************************************/

void wr_cmd (unsigned char c) 
{
	LCD_RS(0)
	LCD_RD(1)
	lcd_send(c);
	LCD_WR(0)
	wait();
	LCD_WR(1)
	wait();
}


/*******************************************************************************
* Write data to LCD controller                                                 *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat (unsigned short c) 
{
	LCD_RS(1)
	LCD_RD(1)
	lcd_send(c);
	LCD_WR(0)
	wait();
	LCD_WR(1)
	wait();
}

/*******************************************************************************
* Read data from LCD controller                                                *
*   Parameter:                                                                 *
*   Return:               read data                                            *
*******************************************************************************/

static __inline unsigned short rd_dat (void) 
{
	unsigned short val = 0;

	LCD_RS(1)
	wait();
	LCD_WR(1)
	LCD_RD(0)
	val = lcd_read();
	LCD_RD(1)
	wait();
	return val;
}

/*******************************************************************************
* Start of data writing to LCD controller                                      *
*   Parameter:                                                                 *
*   Return:                                                                    *
****************************f***************************************************/

static __inline void wr_dat_start (void) 
{
	LCD_RS(1)
}


/*******************************************************************************
* Stop of data writing to LCD controller                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

static __inline void wr_dat_stop (void) 
{
	LCD_CS(1)
}


/*******************************************************************************
* Data writing to LCD controller                                               *
*   Parameter:    c:      data to be written                                   *
*   Return:                                                                    *
*******************************************************************************/

void wr_dat_only (unsigned short c) 
{
	lcd_send(c);
	LCD_WR(0)
	wait_delay(10);
	LCD_WR(1)
	wait_delay(10);
}

/*******************************************************************************
* Read from LCD register                                                       *
*   Parameter:    reg:    register to be read                                  *
*   Return:               value read from register                             *
*******************************************************************************/

static unsigned short rd_reg (unsigned short reg) 
{
	unsigned short val = 0;

	LCD_CS(0)
	wr_cmd(reg);
	wait_delay(5);
	val = rd_dat();  
	val = rd_dat(); 
	LCD_CS(1)
	return (val);
}


/*******************************************************************************
* Write to LCD register                                                        *
*   Parameter:    reg:    register to be read                                  *
*                 val:    value to write to register                           *
*******************************************************************************/

static __inline void wr_reg (unsigned char reg, unsigned short val) 
{
	LCD_CS(0)
	wr_cmd(reg);
	wait_delay(1);
	wr_dat(val);
	LCD_CS(1)
	wait_delay(1);
}

/************************ Exported functions **********************************/

/*******************************************************************************
* Initialize the Graphic LCD controller                                        *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Init (void) 
{
	int i;
	uint16_t rd[32],reg;
	
	
	/* Configure the LCD Control pins */
	DATA_MSB_GPIO->FIODIR |= (0xFF<<LCD_MSB_OFFSET);
	DATA_LSB_GPIO->FIODIR |= (0xFF<<LCD_LSB_OFFSET);
	
	LCD_CS_GPIO->FIODIR |=  PIN_CS;
	LCD_RS_GPIO->FIODIR |=  PIN_RS;
	LCD_WR_GPIO->FIODIR |=  PIN_WR;
	LCD_RD_GPIO->FIODIR |=  PIN_RD;
	
	LCD_CS(1);
	LCD_RS(1);
	LCD_WR(1);
	LCD_RD(1);
	LCD_RST(1);
	
	/*
	delayms(1);
	LCD_RST(0);
	delayms(10);             
	LCD_RST(1);
	delayms(150);
	*/	
	DeviceCode = rd_reg(0x0000); 

	if (DeviceCode == 0x52)	//HX8352A
	{
		wr_reg(0x83, 0x02);   //TESTM=1 
							 
		wr_reg(0x85, 0x03);  	//VDC_SEL=011
		wr_reg(0x8B, 0x01);  
		wr_reg(0x8C, 0x93);  	//STBA[7]=1,STBA[5:4]=01,STBA[1:0]=11
					
		wr_reg(0x91, 0x01);  	//DCDC_SYNC=1
					
		wr_reg(0x83, 0x00);  	//TESTM=0
		
		//Gamma Setting
		wr_reg(0x3E, 0xB0);  

		wr_reg(0x3F, 0x03);  
		wr_reg(0x40, 0x10);  
		wr_reg(0x41, 0x56);  
		wr_reg(0x42, 0x13);  
		wr_reg(0x43, 0x46);  
		wr_reg(0x44, 0x23);  
		wr_reg(0x45, 0x76);  
		wr_reg(0x46, 0x00);
		wr_reg(0x47, 0x5E);  
		wr_reg(0x48, 0x4F);  
		wr_reg(0x49, 0x40);  	
		
		//Power On
					
		wr_reg(0x17, 0x91);  
				 
		wr_reg(0x2B, 0xF9);
		delayms(5);
					
		wr_reg(0x1B, 0x14);  
					
		wr_reg(0x1A, 0x11);  
								
		wr_reg(0x1C, 0x06);  
					
		wr_reg(0x1F, 0x42);  
		delayms(3);
					
		wr_reg(0x19, 0x0A);  
			 
		wr_reg(0x19, 0x1A);  
		delayms(5);
					
					
		wr_reg(0x19, 0x12);  
		delayms(5);
					
		wr_reg(0x1E, 0x27);  
		delayms(10);	   
					
					
		// DISPLAY ON
					
		wr_reg(0x24, 0x60);  
					
		wr_reg(0x3D, 0x40);  
					
		wr_reg(0x34, 0x38);  
					
		wr_reg(0x35, 0x38);  
					
		wr_reg(0x24, 0x38);  
		delayms(10);
					
		wr_reg(0x24, 0x3C);  

#ifdef ORIENTATION_VERTICAL
		wr_reg(0x16, 0xC8);	
#else
		wr_reg(0x16, 0xA8);
#endif
		//wr_reg(0x16, 0x1C);  
					
		wr_reg(0x01, 0x06);  
					
		wr_reg(0x55, 0x00);   

		wr_reg(0x02, 0x00);           
		wr_reg(0x03, 0x00);           
		wr_reg(0x04, 0x00);           
		wr_reg(0x05, 0xef);           
					
		wr_reg(0x06, 0x00);           
		wr_reg(0x07, 0x00);           
		wr_reg(0x08, 0x01);           
		wr_reg(0x09, 0x8f);           
	}
	else if (DeviceCode == 0x65)	//HX8352B
	{
		wr_reg(0x001A, 0x0004); // BT[3:0]=0100, VCL=-VCI; VGH=VCI+2DDVDH; VGL=-2DDVDH
		wr_reg(0x001B, 0x000C); // VRH[4:0]=0Ch, VREG1=(2.5v*1.9)=4.75V		VCIRE=1; 
		// VCOM offset
		wr_reg(0x0023, 0x0000); // SELVCM=0, R24h and R25h selects VCOM
		wr_reg(0x0024, 0x0040); // VCM[6:0]=1000000, VCOMH voltage=VREG1*0.748 (originally 5F)
		wr_reg(0x0025, 0x000F); // VDV[4:0]=01111, VCOMH amplitude=VREG*1.00
		wr_reg(0x002D, 0x0006); // NOW[2:0]=110, Gate output non-overlap period = 6 clocks
		delayms(20);
		// Power on Setting
		wr_reg(0x0018, 0x0008); // RADJ[3:0]=1000, Display frame rate 60Hz 100%
		wr_reg(0x0019, 0x0001); // OSC_EN=1, start OSC
		delayms(20);
		wr_reg(0x0001, 0x0000); // DSTB=0, out deep sleep
		wr_reg(0x001F, 0x0088); // STB=0
		wr_reg(0x001C, 0x0006); // AP[2:0]=110, High OPAMP current (default 011)
		delayms(10);
		wr_reg(0x001F, 0x0080); // DK=0
		delayms(10);
		wr_reg(0x001F, 0x0090); // PON=1
		delayms(5);
		wr_reg(0x001F, 0x00D0); // VCOMG=1
		delayms(10);
		wr_reg(0x0017, 0x0005); // IFPF[2:0]=101, 16-bit/pixel
		
		// Panel Configuration
		//wr_reg(0x0036, 0x0011); // REV_PANEL=1, SM_PANEL=1, GS_PANEL=1, SS_PANEL=1
		//wr_reg(0x0029, 0x0031); // NL[5:0]=110001, 400 lines
		//wr_reg(0x0071, 0x001A); // RTN0


		//Gamma 2.2 Setting
		
		wr_reg(0x0040, 0x0000);	
		wr_reg(0x0041, 0x0009);
		wr_reg(0x0042, 0x0012);
		wr_reg(0x0043, 0x0004);
		wr_reg(0x0044, 0x0000);
		wr_reg(0x0045, 0x0023);//
		wr_reg(0x0046, 0x0003);
		wr_reg(0x0047, 0x005E);//
		wr_reg(0x0048, 0x0000);
		wr_reg(0x0049, 0x0000);
		wr_reg(0x004A, 0x0000);
		wr_reg(0x004B, 0x0000);
		wr_reg(0x004C, 0x0000);
		wr_reg(0x004D, 0x0000);
		wr_reg(0x004E, 0x0000);
		wr_reg(0x0057, 0x004F);//

#ifdef ORIENTATION_VERTICAL
		wr_reg(0x0016, 0x00C8);	
#else
		wr_reg(0x0016, 0x00A8);//A8
#endif
		wr_reg(0x0028, 0x0038); //GON=1; DTE=1; D[1:0]=10
		delayms(40);
		wr_reg(0x0028, 0x003C); //GON=1; DTE=1; D[1:0]=11
	}
	delayms(10);
}

/*
*********************************************************************************************************
* Description: 	Setting the coordinate of cursor
* Arguments  : 	(Xpos , Ypos) the coordinate
* Returns    : 	None
*********************************************************************************************************
*/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	 wr_reg(0x0080, Xpos>>8);
	 wr_reg(0x0081, Xpos&0xFF);
	 wr_reg(0x0082, Ypos>>8);
	 wr_reg(0x0083, Ypos&0xFF); 
}


void LCD_Window(u16 Xstart, u16 Ystart, u16 Xend, u16 Yend)
{
	 wr_reg(0x0002, Xstart>>8);
	 wr_reg(0x0003, Xstart&0xFF);
	 wr_reg(0x0004, Xend>>8);
	 wr_reg(0x0005, Xend&0xFF);
	
	 wr_reg(0x0006, Ystart>>8);
	 wr_reg(0x0007, Ystart&0xFF);
	 wr_reg(0x0008, Yend>>8);
	 wr_reg(0x0009, Yend&0xFF);	

	if (DeviceCode==0x65)
	{	
#ifdef ORIENTATION_VERTICAL
	LCD_SetCursor(Xstart , Ystart);	
#else
	LCD_SetCursor(Ystart , Xstart);
#endif
	}
}


/*******************************************************************************
* Set draw window region to whole screen                                       *
*   Parameter:                                                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_WindowMax (void) 
{
	LCD_Window(0, 0, WIDTH-1, HEIGHT-1);
}


void LCD_PutPixel (u16 x, u16 y, u16 color) 
{
	if (DeviceCode==0x65)
	{
#ifdef ORIENTATION_VERTICAL
		LCD_SetCursor(x, y);
#else
		LCD_SetCursor(y, x);
#endif	
	}
	else
	{
		LCD_Window(x, y, x, y);
	}
	LCD_CS(0)	
	wr_cmd(0x22);  					
	LCD_RS(1);
	wr_dat_only(color);
	LCD_CS(1)	
}


/*******************************************************************************
* Clear display                                                                *
*   Parameter:      color:    display clearing color                           *
*   Return:                                                                    *
*******************************************************************************/

void LCD_Clear (unsigned short color) 
{
	unsigned int   y,x;

	LCD_WindowMax();

	LCD_CS(0);			  
	wr_cmd(0x22);
	LCD_RS(1);
	
	for(y = 0; y <HEIGHT; y++)
	{
		for (x=0; x<WIDTH; x++)
		{
			wr_dat_only(color);
		}	
	}	
	
	LCD_CS(1);	
}

/*
*********************************************************************************************************
* Description: 	In designated areas within the specified color display 
* Arguments  : 	(xsta ,ysta) start coordinate
*				(xend ,yend) end   coordinate
*				color        I don't know 
* Returns    : 	None
*********************************************************************************************************
*/
void LCD_Fill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{                    
    u32 x,y;
											
	LCD_Window(xsta, ysta, xend, yend);		
	LCD_CS(0);
	wr_cmd(0x22);  					
	LCD_RS(1);	 

	for (y=0; y<yend-ysta+1; y++)
	{
		for (x=0; x<xend-xsta+1; x++)
		{
			wr_dat_only(color);
		}
	}	
	
	LCD_CS(1);
	LCD_WindowMax(); 				
} 


/*******************************************************************************
* Set foreground color                                                         *
*   Parameter:      color:    foreground color                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetTextColor (unsigned short color) 
{
  TextColor = color;
}


/*******************************************************************************
* Set background color                                                         *
*   Parameter:      color:    background color                                 *
*   Return:                                                                    *
*******************************************************************************/

void LCD_SetBackColor (unsigned short color) 
{
  BackColor = color;
}

u32 LCD_Pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}

void LCD_Show_Image(int x0, int y0, int w, int h, const u16 *code)
{
	u32 xcounter, ycounter;

	LCD_Window(x0 ,y0, x0+w-1, y0+h-1);
	LCD_CS(0);
	wr_cmd(0x22);  					
	LCD_RS(1);
	
	for(ycounter=0;ycounter<h;ycounter++)
	{
		for(xcounter=0; xcounter<w; xcounter++)
		{
			wr_dat_only(*code++);
		}
	}
	LCD_WindowMax();
}

void LCD_ShowChar(u16 x, u16 y, u8 num, u8 size, u8 mode)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u16 colortemp=POINT_COLOR;      			     
	   
	num=num-' ';
	if(!mode)
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //1206
			else temp=asc2_1608[num][t];		 //1608	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80) 
					LCD_PutPixel(x, y, colortemp);
				else 
					LCD_PutPixel(x, y, BACK_COLOR);
				temp<<=1;
				y++;
				if(x>=WIDTH){POINT_COLOR=colortemp;return;}
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=WIDTH){POINT_COLOR=colortemp;return;}
					break;
				}
			}  	 
	    }    
	}else
	{
	    for(t=0;t<size;t++)
	    {   
			if(size==12)temp=asc2_1206[num][t];  //1206
			else temp=asc2_1608[num][t];		 //1608	                          
	        for(t1=0;t1<8;t1++)
			{			    
		        if(temp&0x80) LCD_PutPixel(x,y, POINT_COLOR); 
				temp<<=1;
				y++;
				if(x>=HEIGHT){POINT_COLOR=colortemp;return;}
				if((y-y0)==size)
				{
					y=y0;
					x++;
					if(x>=WIDTH){POINT_COLOR=colortemp;return;}
					break;
				}
			}  	 
	    }     
	}
	POINT_COLOR=colortemp;	    	   	 	  
} 

void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p)
{         
	u8 x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))
    {       
        if(x>=width){x=x0;y+=size;}
        if(y>=height)break;
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t, y, '0', size, mode&0X01);  
				else LCD_ShowChar(x+(size/2)*t,y, ' ', size, mode&0X01);  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
	}
} 

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1;
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x;
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )
	{  
		LCD_PutPixel(uRow, uCol, POINT_COLOR);
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}    

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

void Draw_Circle(u16 x0,u16 y0,u8 r)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);
	while(a<=b)
	{
		LCD_PutPixel(x0+a, y0-b, POINT_COLOR);             //5
 		LCD_PutPixel(x0+b, y0-a, POINT_COLOR);             //0           
		LCD_PutPixel(x0+b, y0+a, POINT_COLOR);             //4               
		LCD_PutPixel(x0+a, y0+b, POINT_COLOR);             //6 
		LCD_PutPixel(x0-a, y0+b, POINT_COLOR);             //1       
 		LCD_PutPixel(x0-b, y0+a, POINT_COLOR);             
		LCD_PutPixel(x0-a, y0-b, POINT_COLOR);             //2             
  		LCD_PutPixel(x0-b, y0-a, POINT_COLOR);             //7     	         
		a++;
		
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

//END