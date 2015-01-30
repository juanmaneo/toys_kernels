OUTPUT_FORMAT(binary)
STARTUP(PoorOS/start.o)
ENTRY(__start)
SECTIONS
{
.text :
  {
  * (.text*)
  * (.gnu.linkonce.t.*)     /* C++ templates text ? */
  * (.rodata*)              /* read-only data (ELF only) */
  * (.gnu.linkonce.r.*)     /* C++ templates read-only data ? */
  . = ALIGN(4);
  __CTOR_BEGIN__ = .;
  *(SORT(.ctor*))
  __CTOR_END__ = .;
  __DTOR_BEGIN__ = .;
  *(SORT(.dtor*))
  __DTOR_END__ = .;
  . = ALIGN(4);
  }
.data :
  {
  * (.data*)
  * (.gnu.linkonce.d.*)     /* C++ templates data ? */
  . = ALIGN(4);
  }
.bss :
  {
  * (.bss*)
  * (.gnu.linkonce.b.*)     /* C++ templates bss ? */
  * (COMMON)                /* "common" variables */
  . = ALIGN(4);
  }
}

