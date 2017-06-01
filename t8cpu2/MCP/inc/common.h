#include <stdint.h>
#include "MC68F375.h"

#define BDMSUCC __asm("move.l #1, %d0\n bgnd");
#define BDMFAIL __asm("move.l #0, %d0\n bgnd");

#define RamBuf    0x80000
#define RamBufLen 0x00404  /* Address (4 bytes) + 1024 bytes */


void Syscfg();
void swsr();               /* Service watchdog unconditionally */
void Delay(uint32_t del);  /* .. */
void Shadow(uint8_t state);/* Internal function used only by other functions */
void FormatFlash();        /* Format selected partitions */
void WriteFlash();         /* Write flash */

