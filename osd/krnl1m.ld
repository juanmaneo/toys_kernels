/* let the linker use its 'native' format (ELF/COFF/PE)
OUTPUT_FORMAT("coff-go32") */
/* no leading underscore for symbols handled in asm: */
ENTRY(entry)
LS_Phys = 0x100000; /* 1 meg = load (physical) address */
LS_Virt = 0x100000; /* 1 meg = virtual address */
SECTIONS
{
    .text LS_Virt : AT(LS_Phys)
    {
	LS_Code = .;
/* symbols to mark start of code segment */
	code = .; _code = .;
/* kernel code */
	*(.text)
/* .rodata is the ELF constant data section */
	*(.rodata*)
	. = ALIGN(4096);
    }
    .data : AT(LS_Phys + (LS_Data - LS_Code))
    {
	LS_Data = .;
/* symbols to mark start of data segment */
	data = .; _data = .;
/* kernel data */
	*(.data)
	. = ALIGN(4096);
    }
    .bss : AT(LS_Phys + (LS_Bss - LS_Code))
    {
	LS_Bss = .;
/* symbols to mark start of BSS segment */
	bss = .; _bss = .;
/* kernel BSS */
	*(.bss)
	*(COMMON) /* "common" variables */
	. = ALIGN(4096);
    }
/* bug in MinGW? I get a bad executable file unless these
sections are here... */
    .stab :
    {
	*(.stab)
    }
    .stabstr :
    {
	*(.stabstr)
    }
/* symbols to mark end of kernel */
    end = .; _end = .;
}
