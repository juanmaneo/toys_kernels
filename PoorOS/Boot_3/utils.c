/*--------------------------------------------------------------------------*/

#include "utils.h"
#include "txtScreen.h"
#include "keyboard.h"

void *
memcpy(void * dest,
       const void * src,
       size_t n)
{
byte * d=(byte *)dest;
const byte * s=(const byte *)src;
while(n--)
  {
  *(d++)=*(s++);
  }
return dest;
}

void *
memmove(void * dest,
        const void * src,
        size_t n)
{
byte * d=(byte *)dest;
const byte * s=(const byte *)src;
if(d<s)
  {
  while(n--)
    {
    *(d++)=*(s++);
    }
  }
else
  {
  d+=n;
  s+=n;
  while(n--)
    {
    *(--d)=*(--s);
    }
  }
return dest;
}

void *
memset(void * dest,
       int c,
       size_t n)
{
byte * d=(byte *)dest;
while(n--)
  {
  *(d++)=(byte)c;
  }
return dest;
}

size_t
strlen(const char * s)
{
const char * d=s;
while(*(d++));
return d-s-1;
}

char *
strcpy(char * dest,
       const char * src)
{
char * d=dest;
while((*(d++)=*(src++)));
return dest;
}

char *
strncpy(char * dest,
        const char * src,
        size_t n)
{
char * d=dest;
while(n--&&(*(d++)=*(src++)));
return dest;
}

char *
strcat(char * dest,
       const char * src)
{
char * d=dest;
while(*(d++));
--d;
while((*(d++)=*(src++)));
return dest;
}

char *
strncat(char * dest,
        const char * src,
        size_t n)
{
char * d=dest;
while(*(d++));
--d;
while(n--&&(*(d++)=*(src++)));
*(--d)='\0';
return dest;
}

int
strcmp(const char * s1,
       const char * s2)
{
do
  {
  if(*s1<*s2) return -1;
  if(*s1>*s2) return 1;
  ++s1;
  ++s2;
  } while(*s1||*s2);
return 0;
}

extern byte * const __memBegin;
extern byte * const __memEnd;

static bool memInitDone=false;

typedef union
  {
  struct
    {
    size_t prevSize; 
    size_t size;
    bool first;
    bool last;
    bool free;
    } mem;
  byte align[16];
  } MemInfo;

static
void
initMem(void)
{
MemInfo * info=(MemInfo *)__memBegin;
info->mem.prevSize=0;
info->mem.size=(__memEnd-__memBegin)>>4;
info->mem.first=true;
info->mem.last=true;
info->mem.free=true;
memInitDone=true;
}

void *
malloc(size_t size)
{
MemInfo * info;
size_t required;
if(!memInitDone) initMem();
required=(size>>4)+(size&0x0F ? 2 : 1);
for(info=(MemInfo *)__memBegin;;info+=info->mem.size)
  {
  if(info->mem.free)
    {
    if(info->mem.size==required)
      {
      info->mem.free=false;
      return info+1;
      }
    if(info->mem.size>required)
      {
      MemInfo * nextInfo=info+required;
      nextInfo->mem.prevSize=required;
      nextInfo->mem.size=info->mem.size-required;
      nextInfo->mem.first=false;
      nextInfo->mem.last=info->mem.last;
      nextInfo->mem.free=true;
      if(!nextInfo->mem.last)
        {
        (nextInfo+nextInfo->mem.size)->mem.prevSize=nextInfo->mem.size;
        }
      info->mem.size=required;
      info->mem.last=false;
      info->mem.free=false;
      return info+1;
      }
    }
  if(info->mem.last)
    {
    return (void *)0;
    }
  }
}

void
free(void * mem)
{
MemInfo * info=((MemInfo *)mem)-1;
MemInfo * prevInfo=info->mem.first ? (MemInfo *)0 : info-info->mem.prevSize;
MemInfo * nextInfo=info->mem.last ? (MemInfo *)0 : info+info->mem.size;
if(prevInfo&&prevInfo->mem.free)
  {
  if(nextInfo&&nextInfo->mem.free) /* merge with prevInfo & nextInfo */
    {
    prevInfo->mem.size+=info->mem.size+nextInfo->mem.size;
    prevInfo->mem.last=nextInfo->mem.last;
    if(!nextInfo->mem.last)
      {
      (nextInfo+nextInfo->mem.size)->mem.prevSize=prevInfo->mem.size;
      }
    }
  else /* merge with prevInfo */
    {
    prevInfo->mem.size+=info->mem.size;
    prevInfo->mem.last=info->mem.last;
    if(nextInfo)
      {
      nextInfo->mem.prevSize=prevInfo->mem.size;
      }
    }
  }
else
  {
  if(nextInfo&&nextInfo->mem.free) /* merge with nextInfo */
    {
    info->mem.size+=nextInfo->mem.size;
    info->mem.last=nextInfo->mem.last;
    info->mem.free=true;
    if(!nextInfo->mem.last)
      {
      (nextInfo+nextInfo->mem.size)->mem.prevSize=info->mem.size;
      }
    }
  else /* no merge needed */
    {
    info->mem.free=true;
    }
  }
}

void
showAlloc(void)
{
MemInfo * info;
size_t size;
size_t nbUsed=0;
size_t totalUsed=0;
size_t nbFree=0;
size_t totalFree=0;
size_t nbLines=0;
dword column,line;
if(!memInitDone) initMem();
getTxtPos(&column,&line);
if(column)
  {
  writeChar('\n');
  }
for(info=(MemInfo *)__memBegin;;info+=info->mem.size)
  {
  if(++nbLines==getTxtHeight())
    {
    writeStr("--More--");
    do
      {
      while(!inputReady());
      switch(getChar())
        {
        case ' ':
          {
          nbLines=0;
          break;
          }
        case '\n':
          {
          --nbLines;
          break;
          }
        }
      } while(nbLines==getTxtHeight());
    writeChar('\r');
    }
  writeStr("Memory at ");
  writeHexaDWord((dword)info);
  writeStr(": ");
  size=(info->mem.size-1)<<4;
  writeDecimalWord(size);
  if(info->mem.free)
    {
    ++nbFree;
    totalFree+=size;
    writeStr(" bytes free\n");
    }
  else
    {
    ++nbUsed;
    totalUsed+=size;
    writeStr(" bytes used\n");
    }
  if(info->mem.last)
    {
    break;
    }
  }
writeStr("Total: used=");
writeDecimalDWord(totalUsed);
writeStr(" (");
writeDecimalDWord(nbUsed);
writeStr(") free=");
writeDecimalDWord(totalFree);
writeStr(" (");
writeDecimalDWord(nbFree);
writeStr(")\n");
}

/*--------------------------------------------------------------------------*/
