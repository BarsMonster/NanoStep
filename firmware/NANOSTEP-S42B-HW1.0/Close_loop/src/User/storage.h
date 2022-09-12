/*
 * Handles flash memory storage
 * 
 * Contributors:
 * - Jan Swanepoel
 * 
 * 
 * FLASH STORAGE LAYOUT
 * The first 63kB holds the program code.
 * The next 1kB holds the parameters.
 * Calibration table is no longer needed
 * 
 * Parameter Layout
 * HalfWord     Description
 * ----------   -----------
 * 0            Calibration Flag. Calibrated = 0xBB
 * 1            Motor Current
 * 2            
 * 3            Step Size
 * 4
 * 5            Motor Enable Mode
 * 6
 * 7            Motor Direction
 * 
 */
#include "flash.h"
#include "main.h"

#define STM32_FLASH_SIZE 64
#define STM32_FLASH_BASE        0x8000000           // SOC Flash Base Address 

//Moving data store to the end of 64kB
#define Data_Store_Address      0x800FC00           // Flash data storage address (last page of first 64kB, make sure program size stays below it!)  

#if STM32_FLASH_SIZE < 256
    #define STM_SECTOR_SIZE 1024
#else 
    #define STM_SECTOR_SIZE	2048
#endif		 


void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);
void STMFLASH_Write_NoCheck(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);
