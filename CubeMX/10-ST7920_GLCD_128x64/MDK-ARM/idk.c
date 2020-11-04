/************************************************************************
*   LCD graphics driver for Digole 12864w with ST7920 driver using       *
*   CCS software. May work with other ST7920 driven LCD. It has          *
*   the following Pin assignments                                        *
*                                                                        *
*   Pin 1 ------------> Gnd                                              *   
*   Pin 2 ------------> +5volts                                          *
*   Pin 3 ------------> Contrast                                         *
*   Pin 4 ------------> Register Select                                  *
*   Pin 5 ------------> Read/Write                                       *
*   Pin 6 ------------> Enable                                           *
*   Pin 7-14 ---------> Data bits                                        *
*   Pin 15 -----------> PSB (parallel=high & serial=low)                 *
*   Pin 16 -----------> NoConnection                                     *
*   Pin 17 -----------> Reset                                            *
*   Pin 18 -----------> Vout                                             *
*   Pin 19 -----------> +5volts                                          *
*   Pin 20 -----------> Gnd                                              *
*                                                                        *
*   This driver is using 8bit parallel operation for optimal RAM         *
*   requirements, if you want i can change it to 4 bit operation.        *
*                                                                        *
*   Developed by Christian Rebogio                                       *
*   suggestions and request please email me at                           *
*   christian.rebogio@gmail.com                                          *
*                                                                        *
* READ THE DATASHEET FOR ST7920!!!!!!!                                   *
************************************************************************/
// change the pin assignment depending on your circuit

#define  rs  PIN_C0                    //COMMNAD/DATA SELECT
#define  rw  PIN_C1                    //READ/WRITE SELECT             
#define  e   PIN_C2                    //ENABLE SIGNAL                 
#define  rst PIN_C5                    //RESET SIGNAL   

#define ON   1
#define OFF   0
#define XVAL 16  // 16 X 16 or 256 for there is 8 word values for the upper and lower
#define YVAL 32

// PSB is tied to Vcc for this driver because this driver uses Parallel
// operation.
// data is sent using port B so change output_b() to other ports you
//want to use. Dont for get to change the Busy pin @ lcd_check_busy

#define GLCD_WIDTH   128



//////////////////////////////////////////////////////////////////////////////////
//The following are the functions included in this driver file
// glcd_readbyte();
// glcd_instruction( instruction );
// glcd_data( data ); - data can be an array of characters!
// glcd_check_busy();
// glcd_update(); -must be called always after writing a pixel or using functions
//                 from GRAPHICS.C .. Only applicaticable in Graphing mode
// glcd_fillscreen( ON or OFF);
// glcd_init_graph(); initialize for graphing mode
// glcd_init_basic(); initilize for accessing the stored Characters
//                     you can use glcd_data() for writing text
// glcd_pixel(x coordinate, y coordinate, ON or OFF); 
//            -WORKS WITH GRAPHIC.C  from CCS Drivers
// glcd_plot_image(width,height,X coor, Y coor, inverse);
//            -plots the image[] array. Declare it first before this driver.
//             or modify this driver
//
//////////////////////////////////////////////////////////////////////////////////



typedef union
{
  int16 word;
  int8 nbyte[2];
} Dots;

typedef struct
{
  int1 refresh;
  Dots pix[YVAL][XVAL];   // Max dimensions for display (x,y) = (128,32)
  } GD_RAM;             //  (0,0) corresponds to upper lefthand corner.

GD_RAM gdram;


unsigned int8 glcd_readByte (unsigned int1 address)
{
  unsigned int8 data;   // Stores the data read from the LCD
  if(address==1){
     output_high(rs);
  }
  if(address==0){
     output_low(rs);
  }
  output_high(rw);//GLCD_RW = RW_READ;      // Set for reading
  output_high(e);//GLCD_E = 1;      // Pulse the enable pin
  delay_us(1);
  data=input_b();      // Get the data from the display's output register
  output_low(e);//GLCD_E = 0;
  return (data);
}

 
void glcd_check_busy(){
   int1 busy=1;
   output_low(rs);      // LOW RS and High RW will put the lcd to
   output_high(rw);      // read busy flag and address counter
   while(busy){         // will cycle until busy flag is 0
      output_high(e);
      if(!input(PIN_B7)){
         busy=0;
      }
      output_low(e);
   }
}
 
void glcd_instruction(unsigned char x){
   glcd_check_busy();      //must be satisfied before sending instruction
   output_low(rs);      // LOW RS and LOW RW will put the lcd to
   output_low(rw);      // Write instruction mode
   output_b(x);         // 8bit data to bus
   output_high(e);      // enable
   delay_us(1);       
   output_low(e);      // disable
}
void glcd_data(unsigned char x){
   glcd_check_busy();
   output_high(rs);      // HIGH RS and LOW RW will put the lcd to
   output_low(rw);      // Write data register mode
   output_b(x);
   output_high(e);
   delay_us(1);
   output_low(e);
}
 

void glcd_fillScreen (unsigned int1 color)
{
  int8 v, h;
  int16 d;


  d = (color == ON ? 0xFFFFL : 0x0000L);

  for (v=0; v < YVAL; v++)
  {
    for (h=0; h < XVAL; h++)
    {
      gdram.pix[v][h].word = d;
    }
  }
  gdram.refresh = TRUE;
}


void glcd_update ()
{
  int8 v, h;


  if (gdram.refresh)
  {
    for (v=0; v <YVAL; v++)
    {
      glcd_instruction( 0x80 | v);   // Set Vertical Address.
      glcd_instruction( 0x80 | 0);   // Set Horizontal Address.

      for (h=0; h <XVAL; h++)
      {
        glcd_data( gdram.pix[v][h].nbyte[1]);   // Write High Byte.
        glcd_data(  gdram.pix[v][h].nbyte[0]);   // Write Low Byte.
      }
    }
    gdram.refresh = FALSE;
  }
}

void glcd_init_graph(){
   delay_ms(40);
   output_low(rst);         //reset LCD
   delay_us(1);                     
   output_high(rst);        //LCD normal operation
   glcd_instruction(0x30);   //set 8 bit operation and basic instruction set
   delay_us(144);
   glcd_instruction(0x0C);   //display on cursor off and char blink off
   delay_us(100);
   glcd_instruction(0x01);   //display clear
   delay_ms(10);
   glcd_instruction(0x06);   //entry mode set
   delay_us(72);                 
  glcd_instruction(0x34);    // Select extended instruction set.
  delay_ms (10);
  glcd_instruction(0x36);    // Graphic display ON.
  delay_ms (10);

  glcd_fillScreen (OFF);
  glcd_update ();

}

void glcd_init_basic(){
   delay_ms(40);
   output_low(rst);         //reset LCD
   delay_us(1);                     
   output_high(rst);        //LCD normal operation
   glcd_instruction(0x30);   //set 8 bit operation and basic instruction set
   delay_us(144);
   glcd_instruction(0x0C);   //display on cursor off and char blink off
   delay_us(100);
   glcd_instruction(0x01);   //display clear
   delay_ms(10);
   glcd_instruction(0x06);   //entry mode set
   delay_us(72);                 
}

void glcd_pixel(int8 x, int8 y, int1 color)
{
  int8 v, h, b;
  if(y>31){x += 128; y-= 32;};
  v = y;
  h = x/16;
  b = 15 - (x%16);
 
  if (color == ON) bit_set (gdram.pix[v][h].word, b);
  else bit_clear (gdram.pix[v][h].word, b);

  gdram.refresh = TRUE;
}

void glcd_plot_image(int width,int height,int x,int y,int inverse)
{
   unsigned int i=0, j=0, k=0;
   unsigned int16 count=0;
   //glcd_fillScreen(OFF);                        //Clears the screen (opt.)
   for(j=0;j<height;j++)
      {   
         for(;i<width;)
         {
            for(k=8;k>0;k--){
               if(inverse)glcd_pixel(i+x,j+y,~bit_test(image[count],(k-1)));
               else glcd_pixel(i+x,j+y,bit_test(image[count],(k-1)));
               
               i++;
            }
            count++;
         }
      i=0;
      }
}
//credits to http://www.ccsinfo.com/forum/viewtopic.php?t=32819&highlight=st7920//
///////////////////////////////////////////////////////////////////////////////////