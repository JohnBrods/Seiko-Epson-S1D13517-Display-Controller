                                                    /*Anyone is free to copy, modify, publish, use, compile or
                                                    distribute this software, either in source code form or as a compiled
                                                    binary, for non-commercial use only. (i.e. YOU MAY NOT SELL IT)
                                                    John B 17/11/2022
                                                    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
                                                    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
                                                    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
                                                    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
                                                    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
                                                    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
                                                    OTHER DEALINGS IN THE SOFTWARE.*/
                                                    //This C file contains my everything to get a Seiko Epson S1D13517
                                                    //Display Controller to work with a 24 bit colour LCD, 800x480 LCD.
                                                    //Demonstrated on YouTube 18/11/22  and the first of it's kind on YouTube.

//Started Epson Programming 18:30 11/November 2022

unsigned char TFT_DataPort at LATE;
unsigned char TFT_DataPort_Direction at TRISE;  //TRISE is lower PMD 0>7

sbit TFT_RST at LATD0_Bit; //RESET  EPSON CHIP RESET    //EPSON CHIP RESET (WITH PULL-UP) NOT FITTED RESISTOR YET Active low input

sbit TFT_RS  at LATG12_bit;   //THE HD/C# INPUT PIN IS USED TO DISTINGUISH BETWEEN ADDRESS AND DATA

//sbit TFT_CS  at LATD9_BIT; //EPSON CHIP SELECT (WITH PULL-UP) NOT FITTED RESISTOR YET
sbit TFT_CS  at LATG15_bit;  //not used.

sbit TFT_RD  at LATC4_bit;  //PMRD  READ   EBIOE/AN19/RPC4/PMRD/RC4 PIN 13

sbit TFT_WR  at LATC3_bit;  //PMWR  WRITE  EBIWE/AN20/RPC3/PMWR/RC3 PIN 12

sbit TFT_RST_Direction at TRISD0_bit; //RESET  Active low input

sbit TFT_RS_Direction  at TRISG12_bit; //THE HD/C# INPUT PIN IS USED TO DISTINGUISH BETWEEN ADDRESS AND DATA

//sbit TFT_CS_Direction  at TRISD9_BIT;
sbit TFT_CS_Direction  at TRISG15_bit;

sbit TFT_RD_Direction  at TRISC4_bit;  //PMRD  READ

sbit TFT_WR_Direction  at TRISC3_bit;  //PMWR  WRITE  EBIWE/AN20/RPC3/PMWR/RC3

//#define  SD_Card_Chip_Select  LATA9_bit

void Write_Command_EpsonS1D13517(unsigned char Command) {
  TFT_RS = 0;  //RS = 0 NOT DATA  D/C = 0    SET RS LINE LOW FOR COMMAND ON EPSON CHIP
  PMDIN = Command;
  while(PMMODE & 0x8000); //Busy bit
}

void Write_Data_EpsonS1D13517(unsigned int _data) {
  TFT_RS = 1;   //RS = 1 = DATA   D/C = 1    SET RS LINE HIGH FOR DATA  ON EPSON CHIP
  PMDIN = _data;
  while(PMMODE & 0x8000); //Busy bit
}

void Init_16Bit_PMP() {                              // zero wait time is 8 ns strobe @224MHz   1=18.5ns  2=28ns

  PMAEN  = 0;                                                    //  5=55ns @224MHz and sram corrupt   3=36.5 to 37ns =2.240 MHz   4 = 46ns
  PMMODE = 0;
 // PMCONBits.DUALBUF = 1;  //DUAL BUFFER FOR IN AND OUT  NOT AVAILABLE ON ECM
  PMMODEbits.MODE = 2;    //Master 2     Slave =1
  PMMODEbits.WAITB = 0;   //WAITB<1:0>: Data Setup before a Read/Write Strobe Wait States bits(1) 11 = Data wait of 4 TPB; multiplexed address phase of 4 TPB  10 = Data wait of 3 TPB; multiplexed address phase of 3 TPB 01 = Data wait of 2 TPB; multiplexed address phase of 2 TPB 00 = Data wait of 1 TPB; multiplexed address phase of 1 TPB (default)
  PMMODEbits.WAITM = 1;   //1111 = Wait of 16 TPB --0001 = Wait of 2 TPB-- 0000 = Wait of 1 TPB (default)STROBE PULSE WIDTH
  PMMODEbits.WAITE = 0;   //bit 1-0WAITE<1:0>: Data Hold After Read/Write Strobe Wait States bits(1)

  PMMODEbits.MODE16 = 1;  // 1 = 16-bit mode: a read or write to the data register invokes a single 16-bit transfer   For read operations  11 = Wait of 3 TPB  00 = Wait of 0 TPB (default)
  PMCONbits.CSF = 0;      //Chip Select Bit
  PMCONbits.PTRDEN = 1;   //READ STROBE
  PMCONbits.PTWREN = 1;   //WRITE STROBE
  PMCONbits.PMPEN = 1;    //PMP ENABLE
  PMCONbits.ON = 1;
}

void TFT_SET_EpsonS1D13517_5_Inch(){

      TFT_CS_Direction = 0;
      TFT_RST_Direction = 0;
      TFT_RD_Direction = 0;
      TFT_WR_Direction = 0;
      TFT_RS_Direction = 0;
      TFT_DataPort_Direction = 0;
      TFT_RST = 1;
      TFT_RD = 1;
      TFT_WR = 1;
      
      Write_Command_EpsonS1D13517(0x68);
      Write_Data_EpsonS1D13517(0xE8);       //SOFTWARE RESET page 51
      Write_Command_EpsonS1D13517(0x68);
      Write_Data_EpsonS1D13517(0x00);       //SOFTWARE RESET page 51

      /////////////////////////////////////////PAGE 129 FOR EXAMPLES //////////////////////////////////////////////////////////

      Write_Command_EpsonS1D13517(0x04);
      Write_Data_EpsonS1D13517(0x17);      //WAS HEX 0X17 PLLCLK = 1MHz (=24MHz/24=CLKI/PLL_D)         (REG[04h] bit 7 = PLL ENABLE   PAGE 49
      Write_Command_EpsonS1D13517(0x06);
      Write_Data_EpsonS1D13517(0x51);      //VCO = 142-180MHz
      Write_Command_EpsonS1D13517(0x08);   //PLL Divide Select REG 8
      Write_Data_EpsonS1D13517(0x01);      //PLLCLK is divided by 2
      Write_Command_EpsonS1D13517(0x0C);   //PLL N Divider
      Write_Data_EpsonS1D13517(0x59);      //Target Frequency = 90MHz; NN = 180; CLKI = 24MHz; PLL_D = 24:1
      Write_Command_EpsonS1D13517(0x12);   //Clock Source Select Register
      Write_Data_EpsonS1D13517(0x01);
      Write_Command_EpsonS1D13517(0x04);   //Enable PLL
      Write_Data_EpsonS1D13517(0x9D);      //bit 7 enables the ppl
      Write_Command_EpsonS1D13517(0x0E);   //Spread Spectrum modulation enabled with default spread of +/-0.25ns
      Write_Data_EpsonS1D13517(0x3F);
      
      Delay_ms(1);
      
      Write_Command_EpsonS1D13517(0x12);
      Write_Data_EpsonS1D13517(0x82);     //0x92 = 36 MHz 0x81 = 24MHz
      
      //////////////PANEL SETTINGS BELOW ///// REGISTERS AUTO-INCREMENT ///////////// MODE REGISTERS ON PAGE 52 /////////////////////////////////////////////////////////
      
      Write_Command_EpsonS1D13517(0x14);   //LCD Panel Type Register
      Write_Data_EpsonS1D13517(0b00000100);//INPUT IMAGE FORMAT PAGE 61 6 DAYS TO SEE MISTAKE ON EPSON DADA SCREEN 16:34 17TH 11 2022
      //Write_Command_EpsonS1D13517(0x16);   //Horizontal Display Width Register (HDISP) PAGE 44
      Write_Data_EpsonS1D13517(0x64);      //800 + 1 PIXELS WIDE DIVIDE BY 8 = 100  = 100  HEX=64 should be 65?
      //Write_Command_EpsonS1D13517(0x18);   //Horizontal Non-Display Period Register (HNDP)
      Write_Data_EpsonS1D13517(0x4F);
      //Write_Command_EpsonS1D13517(0x1A);   //Vertical Display Height Register 0 (VDISP)
      Write_Data_EpsonS1D13517(0xDF);      //LOW BYTE OF DISPLAY HEIGHT
      //Write_Command_EpsonS1D13517(0x1C);
      Write_Data_EpsonS1D13517(0x01);      //HIGH BYTE OF DISPLAY HEIGHT  1DF = 479
      //Write_Command_EpsonS1D13517(0x1E);   //Vertical Non-Display Period Register (VNDP)
      Write_Data_EpsonS1D13517(0x09);
      //Write_Command_EpsonS1D13517(0x20);   //PHS Pulse Width Register (HSW)
      Write_Data_EpsonS1D13517(0x0F);
      //Write_Command_EpsonS1D13517(0x22);   //PHS Pulse Start Position Register (HPS)
      Write_Data_EpsonS1D13517(0x10);
      //Write_Command_EpsonS1D13517(0x24);   //PVS Pulse Width Register (VSW)
      Write_Data_EpsonS1D13517(0x01);
      //Write_Command_EpsonS1D13517(0x26);   //PVS Pulse Start Position Register (VPS)
      Write_Data_EpsonS1D13517(0x10);
      //Write_Command_EpsonS1D13517(0x28);   //PCLK Polarity Register
      Write_Data_EpsonS1D13517(0x00);

      //////////////SDRAM SETTINGS BELOW //////////////////////////////

      Write_Command_EpsonS1D13517(0x82);
      Write_Data_EpsonS1D13517(0x03);       //SDRAM memory size = 128MBits page 84
      Write_Command_EpsonS1D13517(0x8C);
      Write_Data_EpsonS1D13517(0xFF);       //SDRAM Refresh Rate
      Write_Command_EpsonS1D13517(0x8E);
      Write_Data_EpsonS1D13517(0x03);       //SDRAM Refresh Counter
      Write_Command_EpsonS1D13517(0x90);    //REG[90h] SDRAM Write Buffer Memory Size Register 0 PAGE 86
      Write_Data_EpsonS1D13517(0x40);       //VGA 1M BYTE 40h

      Write_Command_EpsonS1D13517(0x68);
      Write_Data_EpsonS1D13517(0x01);       //Enable SDCLK page 51  When REG[68h] bit 0 = 1b, SDCLK is enabled and all internal clocks may be also enabled.
      
      Write_Command_EpsonS1D13517(0x84);
      Write_Data_EpsonS1D13517(0x82);       //SDRAM refresh page 84

      //TFT_CS = 1;   << not used alway low on my pcb
      Delay_ms(10); // <<< it does need this delay but not sure how long.
}

const code char Tower8x4_bmp[768000];    //Attach this file in your source folder.
unsigned long x;
void Show_Picture(){

    /////// REGISTERS AUTO-INCREMENT ! ///////////////////////FASTER SETUP
    
    Write_Command_EpsonS1D13517(0x5A);    //set window
    Write_Data_EpsonS1D13517(1);          //REG[5Ah] = 00h: Xstart = 0001
    //Write_Command_EpsonS1D13517(0x5C);
    Write_Data_EpsonS1D13517(1);          //REG[5Ch] = 00h, REG[5Eh] = 00h: Ystart = 0001
    //Write_Command_EpsonS1D13517(0x5E);
    Write_Data_EpsonS1D13517(0);          //REG[5Eh] = 00h: Ystart = 0001
    //Write_Command_EpsonS1D13517(0x60);
    Write_Data_EpsonS1D13517(0xC6);       //REG[60h] = C6h: Xend = 800
    //Write_Command_EpsonS1D13517(0x62);
    Write_Data_EpsonS1D13517(0x77);       //REG[62h] = 77h, REG[64h] = 03h: Yend = 480
    //Write_Command_EpsonS1D13517(0x64);    //REG[64h] = 03h: Yend = 480
    Write_Data_EpsonS1D13517(0x03);
    
     ///////////////////////////////  MEMORY ACCESS REGISTERS   ///////////////////////////////////////////////////////////////////////////////////////
    
    //WHEN CNF0 = 0B THE MEMORY DATA PORT (REG[66H] ~ REG[67H]) IS 16-BIT ACCESS. PAGE 23
    //Write_Data_EpsonS1D13517(0x66);       //REG[66h] BITS 7-0MEMORY DATA PORT BITS [7:0]  (WRITE ONLY) THESE BITS SPECIFY THE LSB OF THE DATA WORD.    PAGE 77
    //Write_Command_EpsonS1D13517(0x0);     //REG[67H] BITS 7-0MEMORY DATA PORT BITS [15:8] (WRITE ONLY) THESE BITS SPECIFY THE MSB OF THE DATA WORD.


           for(x = 0; x<768000; x+=2){    //768000

          //  PMDIN = Paris800x480_24bit_bmp[x]  | (Paris800x480_24bit_bmp[x+1]<<8);
            PMDIN = Tower8x4_bmp[x]  | (Tower8x4_bmp[x+1]<<8);
            
           }

}


void main() {

   ANSELA = 0;
   ANSELB = 0;
   ANSELC = 0;
   ANSELD = 0;
   ANSELE = 0;
   ANSELF = 0;
   ANSELG = 0;
   ANSELH = 0;
   ANSELJ = 0;

   TRISA = 0;
   TRISB = 0;
   TRISC = 0;
   TRISD = 0;
   TRISE = 0;
   TRISF = 0;
   TRISG = 0;
   TRISH = 0;
   TRISJ = 0;
   TRISK = 0;

   PORTA=0;PORTB=0;PORTC=0;PORTD=0;PORTE=0;PORTF=0;PORTG=0; PORTH=0; PORTJ=0;  PORTK=0;
   
   JTAGEN_bit = 0;
   
   LATC13_Bit = 1;  //RC13 = LCD DISPLAY RESET //NOT CONNECTED
   LATH15_bit = 1;  //PIN31 LCD DISPLAY (DISP)  DISPLAY ENABLE
   LATD0_bit = 1;   //EPSON CHIP RESET (WITH PULL-UP) NOT FITTED RESISTOR YET
   LATG15_bit = 1;   //EPSON CHIP SELECT (WITH PULL-UP) NOT FITTED RESISTOR YET   LATG15_bit
   
   TFT_CS_Direction = 0;
   TFT_RST_Direction = 0;
   TFT_RD_Direction = 0;
   TFT_WR_Direction = 0;
   TFT_RS_Direction = 0;
   
   Init_16Bit_PMP();
   
   TFT_SET_EpsonS1D13517_5_Inch();
    
   //TFT_CS = 0; //not used on this pcb, always low
    //COLOUR TEST BARS, UN-COMMENT TO USE.
  /*Write_Command_EpsonS1D13517(0x2A);    //Display Mode Register PAGE 64 & 66
   Write_Data_EpsonS1D13517(0b00001111); // PAGE 66  COLOUR BAR   PAGE 123
   Write_Command_EpsonS1D13517(0x50);    // REG[50h] Display Control Register PAGE 72
   Write_Data_EpsonS1D13517(0x80);       //Bit 7 Must Be Set To Update Display*/
   
   Show_Picture();

   Write_Command_EpsonS1D13517(0x2A);
   Write_Data_EpsonS1D13517(0b00000001); //Enable the display   Single buffer display (default) page 65
   Write_Command_EpsonS1D13517(0x50);    //REG[50h] Display Control Register PAGE 72
   Write_Data_EpsonS1D13517(0x80);       //bit 7 must be set to update display

   //The test color bar display does not use the SDRAM memory and is selected when REG[2Ah] bits 3-1 = 111b
   //SET RS LINE LOW FOR COMMAND
   //SET RS LINE HIGH FOR DATA
   
   while(1){

    LATA1_bit = 1;   //TEST LED
    Delay_ms(200);
    LATA1_bit = 0;   //TEST LED
    Delay_ms(200);
  
   }

}