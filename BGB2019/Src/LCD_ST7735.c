#include "stm32f1xx_hal.h"
#include "main.h"
#include "LCD_ST7735.h"
#include "stm32f1xx_hal_spi.h"
// #include "font7x15.h"
SPI_HandleTypeDef hspi2;

extern unsigned long int TIMER_TIC;

void LCD_TXWORD(uint8_t msb,uint8_t lsb);

#define WIDTH  ST7735_TFTWIDTH
#define HEIGHT ST7735_TFTHEIGHT


/********** Draw a word  **********/
void LCD_WORD (const char w[], unsigned char x, unsigned char y, unsigned char field)
// if field > number of chars then clear not printed spaces
{
	unsigned char chars;

	chars=0;
	do {} while (w[chars++]!=0);  //counting number of chars w must finish with /0
	chars--; //do not considere the terminator (0)

	if (field>chars) LCD_COLORWINDOW(x,y,(x+(field*font.pixelW)-1),(y+font.pixelH-1),bkground); //clear field

	if ((justify==RIGHT) && (field>chars)) x+=(font.pixelW)*(field-chars); //adjust x accordind to justify
	if ((justify==CENTER) && (field>chars)) x+=((font.pixelW)*(field-chars))/2; //adjust x accordind to justify
	//no adjustment is necessary if justify==LEFT

	if (chars>0) {
	chars=0;
	do {
		if (w[chars]==' ') LCD_COLORWINDOW (x,y,x+font.pixelW,y+font.pixelH-1,bkground);
		else LCD_CHAR (w[chars],x,y);
		x+=font.pixelW;

	} while (w[++chars]!=0);

	}

}


/********** Draw a int number  -32,768 to 32,767 **********/
void LCD_NUMBER (int n, unsigned char x, unsigned char y, unsigned char field)
// if field > digits then clear not printed spaces
{
	unsigned char digits, sign, aux;
	int nn;

	if (n<0){
		n*=-1;
		sign=1;
	} else {
		sign=0;
	}

	nn=n;			//calculate the number of digits to print removing left zeroes
	digits=0;
do {
	digits++;
} while (nn/=10);
												//allways start printing from lsd
if (justify==RIGHT) x-= font.pixelW;			// correct position in case RIGHT justify
else {
//

	x+=((digits-1) * font.pixelW);
	if (sign==1) x+= font.pixelW;	// don't forget the -
	if ((justify==CENTER) && (field>digits)) x+=((font.pixelW)*(field-digits))/2;
}

//if ((justify==CENTER) && (field>chars)) x+=((font.pixelW)*(field-chars))/2; //adjust x accordind to justify

nn=n;

for (aux = 0; aux < digits; ++aux) {
	LCD_CHAR (('0'+(nn%10)),x,y);
	x-=font.pixelW;
	nn/=10;

}

if (sign==1)
LCD_CHAR ('/',x,y); //print - if negative

x+=font.pixelW*(1-sign);  //now let�s erase non printed characters considering field size
if (field>(digits+sign)){
	if (justify==RIGHT)
		LCD_COLORWINDOW ((x-(font.pixelW*(field-sign-digits))),y,(x-1),(y+font.pixelH-1),bkground);
	else
	if (justify==LEFT)
		LCD_COLORWINDOW ((x+font.pixelW*(sign+digits)),y,(x+(font.pixelW*field)),(y+font.pixelH-1),bkground);
//xxx adicionar para justify = center



}


}


/********** Draw a char  **********/
void LCD_CHAR (unsigned char c, unsigned char x, unsigned char y)
{

	LCD_BMP2 (font.baseAddr+(c-font.ascshift)*font.bytesW*font.pixelH,x,y,font.pixelW,font.pixelH);
}


/********** Draw a 2 colour (1bit) image **********/ 
void LCD_BMP2 (const unsigned char *FIGURE, unsigned char X, unsigned char Y, unsigned char width, unsigned char height)
{
	uint16_t pixs;


	LCD_WINDOW (X,Y,X+width-1,Y+height-1);
	LCD_CMD(0x2C); //memory write
	LCD_spi2_16b(); //color data is 16 bit wide
	LCD_DC_1;
	uint8_t FOREG[]={((unsigned char)(frground & 0x00FF)),((unsigned char)((frground & 0xFF00)>>8))};
	uint8_t BACKG[]={((unsigned char)(bkground & 0x00FF)),((unsigned char)((bkground & 0xFF00)>>8))};
	
	unsigned char bit=0x80;
	unsigned char bitsW=width;
	for (pixs=width * height ; pixs>0 ; pixs--)
	{
		if (*FIGURE & bit) 
		{
			HAL_SPI_Transmit(&hspi2,FOREG,1,0x1);
 			while((SPI2->SR) & SPI_SR_BSY);
		}
		else
		{
			HAL_SPI_Transmit(&hspi2,BACKG,1,0x1);
			while((SPI2->SR) & SPI_SR_BSY);
		}
		bit>>=1;
		bitsW--;
		if (!((bit)&&(bitsW)))
		{
			bit=0x80;
			FIGURE++;
			if (bitsW==0) bitsW=width;

		}

	}
	LCD_spi2_8b(); 
	//
	//

}

/********** Draw a filled circle center xc,yc  **********/
//credits: https://stackoverflow.com/questions/1201200/fast-algorithm-for-drawing-filled-circles/1201304

void LCD_DISK(int xc, int yc, int radius, unsigned int color)
{
    int x = radius;
    int y = 0;
    int xChange = 1 - (radius << 1);
    int yChange = 0;
    int radiusError = 0;

    while (x >= y)
    {
        for (int i = xc - x; i <= xc + x; i++)
        {
            LCD_PLOT(i, yc + y,color);
            LCD_PLOT(i, yc - y, color);
        }
        for (int i = xc - y; i <= xc + y; i++)
        {
        	LCD_PLOT(i, yc + x,color);
        	LCD_PLOT(i, yc - x,color);
        }

        y++;
        radiusError += yChange;
        yChange += 2;
        if (((radiusError << 1) + xChange) > 0)
        {
            x--;
            radiusError += xChange;
            xChange += 2;
        }
    }
}

/********** Draw a 11X11 dot center x,y **********/

void LCD_DOT11 (unsigned char x, unsigned char y, unsigned int color)
{
	LCD_LINE (x-2,y-5, x+2, y-5, color);
	LCD_LINE (x-3,y-4,x+3, y-4,color);
	LCD_LINE (x-4,y-3,x+4, y-3,color);
	LCD_LINE (x-2,y+5,x+2, y+5,color);
	LCD_LINE (x-3,y+4,x+3, y+4,color);
	LCD_LINE (x-4,y+3,x+4, y+3,color);
	LCD_COLORWINDOW (x-5,y-2,x+5,y+2, color);
}

/********** Draw a frame **********/
void LCD_FRAME(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color)
{
	LCD_LINE (x1,y1,x1,y2,color);
	LCD_LINE (x2,y1,x2,y2,color);
	LCD_LINE (x2,y2,x1,y2,color);
	LCD_LINE (x2,y1,x1,y1,color);

}




/********** Draw a time dependent doter line **********/




void LCD_DOTLINE(unsigned char x2, unsigned char y2, unsigned char x1, unsigned char y1, unsigned int color, unsigned int bkcolor, unsigned char densit)
// densit > higher > less dots
{
int   dx, dy, sx, sy;
 char  x,  y, mdx, mdy, l ;
 unsigned long int control;

control=TIMER_TIC/100; // the position of the dots will depend on the timing

  dx=x2-x1; dy=y2-y1;

  if (dx>=0) { mdx=dx; sx=1; } else { mdx=x1-x2; sx=-1; }
  if (dy>=0) { mdy=dy; sy=1; } else { mdy=y1-y2; sy=-1; }

  x=x1; y=y1;

  if (mdx>mdy) {//If the length of the line with respect to x is less than y
     l=mdx;
     while (l>0) {
         if (dy>0) { y=y1+((mdy*(x-x1))/dx); }
            else { y=y1-((mdy*(x-x1))/dx); }
         if (++control % densit)
        	 LCD_PLOT(x,y,bkcolor);
         else
         LCD_PLOT(x,y,color);//Calculate the current x, y and draw pixel by pixel by specifying the color
         x=x+sx;
         l--;
     }
  } else {
     l=mdy;//If the line length in y is less than x
     while (l>0) {
        if (dx>0) { x=x1+((mdx*(y-y1))/dy); }
          else { x=x1-((mdx*(y-y1))/dy); }
        if (++control % densit)
       	 LCD_PLOT(x,y,bkcolor);
        else
        LCD_PLOT(x,y,color);//Calculate the current x, y and draw pixel by pixel by specifying the color
        y=y+sy;
        l--;
     }
  }
  if (++control % densit)
 	 LCD_PLOT(x2,y2,bkcolor);
  else
  LCD_PLOT(x2,y2,color);//Calculate the current x, y and draw pixel by pixel by specifying the color
}
///////


/********** Draw a line **********/ 
// XXX lcd7735_line



void LCD_LINE(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color) 
{
int   dx, dy, sx, sy;
 char  x,  y, mdx, mdy, l;

  if (x1==x2) { // 
	  LCD_COLORWINDOW(x1,y1, x1,y2, color);//If the line is straight then just draw
	  return;
  }

  if (y1==y2) { // 
	  LCD_COLORWINDOW(x1,y1, x2,y1, color);//If the line is straight then just draw
	  return;
  }
//Here calculation of the oblique line drawing 
  dx=x2-x1; dy=y2-y1;

  if (dx>=0) { mdx=dx; sx=1; } else { mdx=x1-x2; sx=-1; }
  if (dy>=0) { mdy=dy; sy=1; } else { mdy=y1-y2; sy=-1; }

  x=x1; y=y1;

  if (mdx>mdy) {//If the length of the line with respect to x is less than y
     l=mdx;
     while (l>0) {
         if (dy>0) { y=y1+((mdy*(x-x1))/dx); }
            else { y=y1-((mdy*(x-x1))/dx); }
         LCD_PLOT(x,y,color);//Calculate the current x, y and draw pixel by pixel by specifying the color
         x=x+sx;
         l--;
     }
  } else {
     l=mdy;//If the line length in y is less than x
     while (l>0) {
        if (dx>0) { x=x1+((mdx*(y-y1))/dy); }
          else { x=x1-((mdx*(y-y1))/dy); }
        LCD_PLOT(x,y,color);//Calculate the current x, y and draw pixel by pixel цвет
        y=y+sy;
        l--;
     }
  }
  LCD_PLOT(x2, y2, color);
}
/********** Plot a single pixel **********/ 
// XXX lcd7735_putpix
void LCD_PLOT(unsigned char X, unsigned char Y, unsigned int color)
{
	// LCD_CS0;
	LCD_WINDOW(X, Y, X, Y);
	LCD_CMD(0x2C);//Memory write
	LCD_DATA((unsigned char)((color & 0xFF00)>>8));
	LCD_DATA((unsigned char) (color & 0x00FF));
 
}
/********** Define a window and fill **********/  
// XXX lcd7735_fillrect
void LCD_COLORWINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2, unsigned int color)
{
	unsigned char msb,lsb;
	int i,dots;
	if (X1>X2) 
	{
		msb=X1;
		X1=X2;
		X2=msb;
	}

	if (Y1>Y2) 
	{
		msb=Y1;
		Y1=Y2;
		Y2=msb;
	}

	dots= (X2-X1+1)*(Y2-Y1+1);
	msb =((unsigned char)((color & 0xFF00)>>8));//most significant bits
	lsb =((unsigned char)(color & 0x00FF));	//less significant bits
	LCD_WINDOW (X1, Y1, X2, Y2);
	LCD_CMD(0x2C); //memory write
	LCD_spi2_16b(); //color data is 16 bit wide
	LCD_DC_1;
	for (i=0;i<dots;i++)
			LCD_TXWORD(msb,lsb);
		
		while((SPI2->SR) & SPI_SR_BSY); // wait for spi2 finish sending
		LCD_spi2_8b(); 

}

/********** Define a window  **********/  
// XXX lcd7735_at
void LCD_WINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2)
{
	LCD_CMD(0x2A); //Dedine verticals
	LCD_DATA(0x00);
	LCD_DATA(X1);
	LCD_DATA(0x00);
	LCD_DATA(X2);
	LCD_CMD(0x2B); //Dedine horizontals
	LCD_DATA(0x00);
	LCD_DATA(Y1);
	LCD_DATA(0x00);
	LCD_DATA(Y2);

}


/********** Initialize LCD controller **********/  
// XXX lcd7735_ini
void LCD_INIT(void)
{
	LCD_RST_0;
	HAL_Delay(10);
	LCD_RST_1;
	HAL_Delay(10);
	LCD_CMD(0x11);
	HAL_Delay(120);
	LCD_CMD (0x3A);		//color mode
	LCD_DATA (0x05);	//16 bit mode
	LCD_CMD (0x36);		//pixel sequence
	LCD_DATA (0xA4);	//bottom to top, right to left, BGR color order
	LCD_CMD (0x29);		//turn display on

}


/********** Send Command to LCD **********/
// XXX lcd7735_sendCmd
void LCD_CMD(unsigned char command)
{
	LCD_DC_0;
	STABILITY2;     // if instability xxxx  testar sem
	LCD_TXBYTE (command);
}



/********** Send Data to LCD **********/
// XXX lcd7735_sendData
void LCD_DATA(unsigned char datum)
{
	LCD_DC_1;
	STABILITY2;     // if instability xxxx testar sem
	LCD_TXBYTE (datum);
}



/********** Init SPI in 8bit mode **********/  
// XXX spi2_8b_init
void LCD_spi2_8b(void)	
{
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(&hspi2);
}


/********** Init SPI in 16bit mode **********/
// XXX spi2_16b_init
void LCD_spi2_16b(void)	
{
	hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	HAL_SPI_Init(&hspi2);
}

/********** SENDING BYTE TO LCD **********/  
// XXX lcd7735_senddata
void LCD_TXBYTE(unsigned char data)    
{
HAL_SPI_Transmit(&hspi2, &data,1,0x1);
}


/********** SENDING 16bits TO LCD **********/
// XXX  lcd7735_send16bData
void LCD_TXWORD(uint8_t msb,uint8_t lsb) 
{
	uint8_t WRD[]={lsb,msb};
	HAL_SPI_Transmit(&hspi2,WRD,1,0x1);
}
