/*
 * Config.h
 *
 * Created: 12/31/2015 11:27:00 AM
 *  Author: Chriva
 */ 
#ifndef CONFIG_H_
#define CONFIG_H_




#ifndef STM32F103RB
//#define _4X20DISP
#define _2X16DISP
#ifndef MCP2515__
#define MCP2515__
#endif
#ifndef AVR
#define AVR
#endif
#else
#define _2X16DISP
#endif




#ifdef _4X20DISP
#define DSPLINES  4
#define DSPCHARS 20 ///< Even 16 char displays seems to be 20x internally; Don't change unless necessary.
#define DSPVISCH 20 ///< Number of visible characters; Usually 16 or 20
#define DSPSPACE 10 ///< How far to the right should symbolname 2 be? (Set this to 10 on 20 char displays, 8 on 16)
#define CHRNOSPC  2 ///< How many steps between symbol name and shown value? (Will automatically be decreased by one if one sets a chosen symbols data length to two bytes.)
#define SYMpLINE  2 ///< How many symbols and data / line should be shown?
#endif

#ifdef _2X16DISP
#define DSPLINES  2
#define DSPCHARS 20
#define DSPVISCH 16 
#define DSPSPACE  8 
#define CHRNOSPC  1 
#define SYMpLINE  2 
#endif





#ifdef AVR
///< PORTs cheats sheet

///< 1 B
///< 2 C
///< 3 D

///< BDM pins
#define P_1 0
#define P_2 0

#define P_RST  3, 2

#define P_BKPT 3, 4
#define P_FRZ  3, 7


#define P_DSI  3, 1
#define P_DSO  3, 0









#define F_CPU 16000000UL

#define SDCARD_CS_PORT          1
#define SDCARD_CS_PIN           0



#define	MCP2515_CS_1			1,2
#define	MCP2515_INT_1			B,1



#define LCD_DB4_PORT            2      
#define LCD_DB5_PORT            2
#define LCD_DB6_PORT            2
#define LCD_DB7_PORT            2

#define LCD_DB4_PIN             0
#define LCD_DB5_PIN             1
#define LCD_DB6_PIN             2
#define LCD_DB7_PIN             3

//#define LCD_RW_PORT             2
#define LCD_E_PORT              2
#define LCD_RS_PORT             3

//#define LCD_RW_PIN              4
#define LCD_E_PIN               5
#define LCD_RS_PIN              3
#endif






#endif /* CONFIG_H_ */
