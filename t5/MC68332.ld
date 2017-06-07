STARTUP(out/startup.o)
OUTPUT_FORMAT("binary")
OUTPUT_ARCH(m68k)

MEMORY {
	ram : ORIGIN = 0x100000, LENGTH = 0x800
}
PROVIDE (__stack = 0x100800);

SECTIONS{
	.text  0x100400  : {
	
		__s_text = . ;	
		*     (.text) 
		CONSTRUCTORS
		__e_text = . ; 
	} > ram	

	.data : {

		__s_data = . ; /* Symbols, to know where to */
		*(.data)
		__e_data = . ; /* copy the data.            */
	} > ram

	.bss : {
	
		__s_bss = . ;
		*(.bss)
		*(COMMON)
		__e_bss = . ;
	} > ram
}
