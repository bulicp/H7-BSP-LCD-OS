Odvisno od lokacije kode (interni flash ali externi flash ter uporabljenih internih RAM-ov) moraš 
izbrati pravi datoteki startup_stm32h750xbhx.s in system_stm32h7xx.c !!!!!


**********************************************************
I N T E R N A L    F L A S H:

  FLASH (rx)     : ORIGIN = 0x08000000, LENGTH = 128K
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
**********************************************************
startup_stm32h750xbhx.s.INTERNAL_FLASH in system_stm32h7xx.c_INTERNAL_FLASH sta za kodo v intenrem flashu. Linker skripta je STM32H750XBHX_FLASH.ld-INTERNAL_FLASH




**********************************************************
E X T E R N A L    F L A S H:

  FLASH (rx)     : ORIGIN = 0x90000000, LENGTH = 131072K
  DTCMRAM (xrw)  : ORIGIN = 0x20000000, LENGTH = 128K
  RAM_D1 (xrw)   : ORIGIN = 0x24000000, LENGTH = 512K
  RAM_D2 (xrw)   : ORIGIN = 0x30000000, LENGTH = 288K
  RAM_D3 (xrw)   : ORIGIN = 0x38000000, LENGTH = 64K
  ITCMRAM (xrw)  : ORIGIN = 0x00000000, LENGTH = 64K
**********************************************************
startup_stm32h750xbhx.s.EXTERNAL_FLASH in system_stm32h7xx.c_EXTERNAL_FLASH sta za kodo v intenrem flashu. Linker skripta je STM32H750XBHX_FLASH.ld-EXTERNAL_FLASH




**********************************************************
Nalaganje v ext flash:

1. Prevedi Templates/ExtMem_Boot in ga naloži v interni flash (kot nalaga linker skripta)
2. Svoj program nato prevedi in ga v externi flash naloži s CubeProgrammer (izberi external bootloader MT25TL01G)




**********************************************************
Debug:
1. V CubeIDE najprej nastavi Debubg Configurations -> Sturtup -> izberi konfiguracijo (verjetno le le ena) Debug/*elf -> Edit: Odkkljukaj Download, saj ne moreš kode naložiti v externi flash s CubeIDE
2. Zaženi Debug

