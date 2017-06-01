/**********************************
* * * * * * Sys stuff * * * * * * *  
**********************************/
/* Synthesizer Control Register(Fastref) */
#define SYNCR			0xFFFA04 

/* Software Watchdog Service Register */
#define SWSR			0xFFFA27


/**********************************
* * * * * Eeprom stuff  * * * * * *  
**********************************/

/* CMFI EEPROM Configuration Register */
#define CMFIMCR			0xFFF800

/* CMFI EEPROM Test Register */
#define CMFITST			0xFFF804

/* Address register (CMFIBA H/L)*/
#define CMFIBAR         0xFFF808

/* CMFI High Voltage Control Reg 1 */
#define CMFICTL1		0xFFF80C

/* CMFI High Voltage Control Reg 2 */
#define CMFICTL2		0xFFF80E