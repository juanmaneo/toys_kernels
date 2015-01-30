OUTPUT_FORMAT(binary)
STARTUP(start.o)
ENTRY(__start)
SECTIONS
{
.text :
  {
  * (.text*)
  * (.rodata*)              /* read-only data (ELF only) */
  . = ALIGN(4);
  }
.data :
  {
  * (.data*)
  . = ALIGN(4);
  }
.bss :
  {
  * (.bss*)
  * (COMMON)                /* "common" variables */
  . = ALIGN(4);
  }
}

