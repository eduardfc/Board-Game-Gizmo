#ifndef FUNC_H
#define FUNC_H


#define ST7735_TFTWIDTH  128
#define ST7735_TFTHEIGHT 160



// defining GPIO Pins to LCD
#define LCD_RST_PORT GPIOA
#define LCD_RST_PIN GPIO_PIN_8
#define LCD_SDA_PORT GPIOB
#define LCD_SDA_PIN GPIO_PIN_15
#define LCD_DC_PORT GPIOB
#define LCD_DC_PIN GPIO_PIN_14
#define LCD_CLK_PORT GPIOB
#define LCD_CLK_PIN GPIO_PIN_13
#define LCD_CS_PORT GPIOB
#define LCD_CS_PIN GPIO_PIN_12
#define RED  0xf800
#define GREEN2 0X7E0
#define	GREEN1 0X1E0
#define COLORP1 0XFE80
#define BLUE 0X1F
#define BLUE0 0X10
#define YELLOW 0XFFE0
#define COLORP2 0X055F
#define BLACK  0x0
#define WHITE 0xffff
#define PINK 0xf999
#define GREY0 0x2124                  //    00100 001001 00100
#define GREY1 0x4A69
#define GREY2 0xDEFB
#define TOPTABLE 0x2442
#define SWOOD 0x8222
#define GOLD 0xFEA0


void LCD_CMD(unsigned char command);
void LCD_DATA(unsigned char datum);
void LCD_spi2_8b(void);
void LCD_spi2_16b(void);

void LCD_TXBYTE(unsigned char data);
void LCD_COLORWINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2, unsigned int color);
void LCD_INIT(void);
void LCD_WINDOW(unsigned char X1, unsigned char Y1, unsigned char X2, unsigned char Y2);
void LCD_PLOT(unsigned char X, unsigned char Y, unsigned int color);
void LCD_LINE(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color);
void LCD_BMP2 (const unsigned char *FIGURE, unsigned char X, unsigned char Y, unsigned char width, unsigned char height);
void LCD_NUMBER (int n, unsigned char x, unsigned char y, unsigned char field);
void LCD_DOT11 (unsigned char x, unsigned char y, unsigned int color);

// definitions to easily set/reset DC or RST pins
#define LCD_RST_1 HAL_GPIO_WritePin(LCD_RST_PORT, GPIO_PIN_8,GPIO_PIN_SET);
#define LCD_RST_0 HAL_GPIO_WritePin(LCD_RST_PORT, GPIO_PIN_8,GPIO_PIN_RESET);

#define LCD_DC_1  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN,GPIO_PIN_SET); 
#define LCD_DC_0  HAL_GPIO_WritePin(LCD_DC_PORT, LCD_DC_PIN,GPIO_PIN_RESET); 
#define STABILITY2 for (int ii=0;ii<100;++ii ); //generate small delay

 typedef struct  {
	const unsigned char *baseAddr; // start of font array
	unsigned char ascshift; // displacement of ascii code for the 1st char
	unsigned char pixelW; // width in pixels
	unsigned char pixelH; // height in pixels
	unsigned char bytesW; // number of bytes per line

} FONTS;

FONTS font;
unsigned int frground;
unsigned int bkground;

char justify;
#define LEFT 1
#define RIGHT 2
#define CENTER 3 // only for text


void LCD_CHAR (unsigned char c, unsigned char x, unsigned char y);

//Display initialization
void LCD_INIT(void);
//Sending data
void lcd7735_sendData(unsigned char data);
//Sending a command
void lcd7735_sendCmd(unsigned char cmd); 
// Procedure for filling the rectangular area of the screen with the specified color
void lcd7735_fillrect(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY, unsigned int color);
// Pixel output
void lcd7735_putpix(unsigned char x, unsigned char y, unsigned int Color);
// Line drawing
void lcd7735_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color);
// Drawing a rectangle (not filled)	 
void lcd7735_rect(char x1,char y1,char x2,char y2, unsigned int color);
//Output the symbol to the screen by coordinates
void lcd7735_putchar(unsigned char x, unsigned char y, unsigned char chr, unsigned int charColor, unsigned int bkgColor);
//Output line		 
void lcd7735_putstr(unsigned char x, unsigned char y, const unsigned char str[], unsigned int charColor, unsigned int bkgColor);
//Large numbers
/* void printDigit (unsigned char posX, unsigned char posY,unsigned char number,unsigned int numberColor,unsigned int bacgroundColor);
//Decimal print		 
void LCD7735_dec(unsigned int numb, unsigned char dcount, unsigned char x, unsigned char y,unsigned int fntColor, unsigned int bkgColor);
// Define the area of the screen to fill

 */
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY) ;
#endif
