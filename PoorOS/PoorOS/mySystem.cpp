
#include "PoorOS/system.h"
#include "PoorOS/console.h"
#include "PoorOS/task.h"

using namespace PoorOS;

//----------------------------------------------------------------------------

class Dummy : public Task
{
public:

  Dummy(void) {}

  virtual
  ~Dummy(void) {}

  virtual
  void
  main(void)
  {
  Console * c=System::theSystem->accessConsole();
  for(;;)
    {
    c->writeChar('.');
    pause(100);
    }
  }
};

//----------------------------------------------------------------------------

class Clock : public Task
{
public:

  Clock(void) {}

  virtual
  ~Clock(void) {}

  virtual
  void
  main(void)
  {
  System * s=System::theSystem;
  Console * c=s->accessConsole();
  dword lastSecond=0;
  for(;;)
    { // Ouch ! busy wait (no explicit scheduling) !!!
    dword second=s->getTime()/1000;
    if(second!=lastSecond)
      {
      lastSecond=second;
      dword row,line;
      c->getCursor(row,line);
      Console::Color color=c->getColor();
      c->setCursor(50,5);
      c->setColor(Console::TXT_LIGHT_RED);
      c->writeStr("== ");
      c->writeDecimalDWord(second/3600);
      c->writeStr("h ");
      second%=3600;
      c->writeDecimalDWord(second/60);
      c->writeStr("m ");
      second%=60;
      c->writeDecimalDWord(second);
      c->writeStr("s ==");
      c->setCursor(row,line);
      c->setColor(color);
      }
    }
  }
};

//----------------------------------------------------------------------------

class Song : public Task
{
public:

  Song(void) {}

  virtual
  ~Song(void) {}

  struct Note
  {
  enum
    {
    MUTE=0,
    DO_4=261,
    RE_4=294,
    MI_4=330,
    FA_4=349,
    SOL_4=392,
    LA_4=440,
    SI_4=494,
    DO_5=523,
    RE_5=587
    };
  dword frequency;
  dword duration;
  };

  virtual
  void play(Song::Note * part)
  {
  Console * c=System::theSystem->accessConsole();
  dword tempo=2;
  for(Note * n=part;n->duration;n++)
    {
    if(n->frequency)
      {
      c->beep(n->frequency,n->duration*tempo);
      }
    else
      {
      pause(n->duration*tempo);
      }
    }
  }

  virtual
  void
  main(void)
  {
  Note part1[]={ { Note::SOL_4,  50 },
                 { Note::LA_4,   50 },
                 { Note::SI_4,   50 },
                 { Note::SOL_4,  50 },
                 { Note::LA_4,   75 },
                 { Note::MUTE,   25 },
                 { Note::LA_4,   50 },
                 { Note::SI_4,   50 },
                 { Note::DO_5,   75 },
                 { Note::MUTE,   25 },
                 { Note::DO_5,  100 },
                 { Note::SI_4,   75 },
                 { Note::MUTE,   25 },
                 { Note::SI_4,  100 },
                 { Note::SOL_4,  50 },
                 { Note::LA_4,   50 },
                 { Note::SI_4,   50 },
                 { Note::SOL_4,  50 },
                 { Note::LA_4,   75 },
                 { Note::MUTE,   25 },
                 { Note::LA_4,   50 },
                 { Note::SI_4,   50 },
                 { Note::DO_5,  100 },
                 { Note::RE_5,  100 },
                 { Note::SOL_4, 200 },
                 { Note::MUTE,    0 } };
  Note part2[]={ { Note::RE_5,   75 },
                 { Note::MUTE,   25 },
                 { Note::RE_5,   50 },
                 { Note::DO_5,   50 },
                 { Note::SI_4,  100 },
                 { Note::LA_4,   50 },
                 { Note::SI_4,   50 },
                 { Note::DO_5,  100 },
                 { Note::RE_5,  100 },
                 { Note::LA_4,  200 },
                 { Note::MUTE,    0 } };
  play(part1);
  play(part2);
  play(part2);
  play(part1);
  }
};

//----------------------------------------------------------------------------

class MySystem : public System
{
public:

  MySystem(void);

  virtual
  ~MySystem(void);

  virtual
  bool
  main(void);
};

MySystem::MySystem(void)
: System()
{
setConsole(new Console);
accessConsole()->writeStr("MySystem is up !\n");
}

MySystem::~MySystem(void)
{
accessConsole()->writeStr("MySystem is down !\n");
}


bool // destroy system
MySystem::main(void)
{
struct ColorMsg
  {
  Console::Color color;
  const char * msg;
  };
ColorMsg colorMsg[]={ { Console::TXT_BLUE, "TXT_BLUE" },
                      { Console::TXT_GREEN, "TXT_GREEN" },
                      { Console::TXT_CYAN, "TXT_CYAN" },
                      { Console::TXT_RED, "TXT_RED" },
                      { Console::TXT_MAGENTA, "TXT_MAGENTA" },
                      { Console::TXT_BROWN, "TXT_BROWN" },
                      { Console::TXT_LIGHT_GREY, "TXT_LIGHT_GREY" },
                      { Console::TXT_GREY, "TXT_GREY" },
                      { Console::TXT_LIGHT_BLUE, "TXT_LIGHT_BLUE" },
                      { Console::TXT_LIGHT_GREEN, "TXT_LIGHT_GREEN" },
                      { Console::TXT_LIGHT_CYAN, "TXT_LIGHT_CYAN" },
                      { Console::TXT_LIGHT_RED, "TXT_LIGHT_RED" },
                      { Console::TXT_PINK, "TXT_PINK" },
                      { Console::TXT_YELLOW, "TXT_YELLOW" },
                      { Console::TXT_WHITE, "TXT_WHITE" },
                      { (Console::Color)0, (const char *)0 } };
Console * c=accessConsole();
Console::Color color=c->getColor();
for(size_t i=0;colorMsg[i].color;i++)
  {
  c->setColor(colorMsg[i].color);
  c->writeStr(colorMsg[i].msg);
  c->writeChar('\t');
  c->writeHexaByte(colorMsg[i].color);
  c->writeChar('\n');
  }
c->setColor(color);
Dummy * dummy=new Dummy;
Clock * clock=new Clock;
Song * song=new Song;
c->writeStr("\nCtrl-D: dummy    Ctrl-C: clock     ");
c->writeStr("Ctrl-S: song    Ctrl-X: memory    Esc: stop\n");
bool stop=false;
while(!stop)
  {
  if(song->ended())
    {
    delete song;
    song=new Song;
    }
  while(c->inputReady())
    {
    char ch=c->getChar();
    Task * task=(Task *)0;
    switch(ch)
      {
      case '\x04': // Ctrl-D
        {
        task=dummy;
        break;
        }
      case '\x03': // Ctrl-C
        {
        task=clock;
        break;
        }
      case '\x13': // Ctrl-S
        {
        task=song;
        break;
        }
      case '\x18': // Ctrl-X
        {
        showAlloc();
        break;
        }
      case '\x1B': // Esc
        {
        stop=true;
        break;
        }
      default:
        {
        c->writeChar(ch);
        }
      }
    if(task)
      {
      if(task->getWakeUpTime()==0xFFFFFFFF)
        {
        task->pause(0);
        }
      else
        {
        task->pause(0xFFFFFFFF);
        }
      }
    }
  schedule(50);
  }
delete dummy;
delete clock;
delete song;
return true;
}

System * System::theSystem=new MySystem;

