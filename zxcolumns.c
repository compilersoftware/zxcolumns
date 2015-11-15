/*---------------------------------------------------------------------

   ZXColumns  -> The Spectrum version of the classic columns game.
   v 1.0         (c) 2005 Santiago Romero AkA NoP / Compiler
                        sromero@escomposlinux.org
 
                 Dedicated to Jesús Ramos :-)
                    
 Feel free to improve this game, solve bugs, and add whatever
 you want. It's free software, you can do it without asking me,
 but it would be very nice if you maintain my name in the
 credits.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

---------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#include "sprites.c"
#include "columns.c"
#include "gfxlib.c"
#define COUNTER_LIMIT 30
#define initial_x 3
#define initial_y 0

#include "fondo.asm"

#define WaitKey getkey


//---------------------------------------------------------------------
#define TextDefault()       printf( "\x1B[m" )
#define TextHiRes()         printf( "%c2J",155 )
#define TextBold()          printf( "\x1B[1m" )
#define TextDim()           printf( "\x1B[2m" )
#define TextUnderline()     printf( "\x1B[4m" )
#define TextUnunderline()   printf( "\x1B[24m" )
#define TextReverse()       printf( "\x1B[7m" )
#define TextUnreverse()     printf( "\x1B[27m" )
#define TextFG_Black()      printf( "\x1B[%um", 30 )
#define TextFG_Red()        printf( "\x1B[%um", 31 )
#define TextFG_Green()      printf( "\x1B[%um", 32 )
#define TextFG_Yellow()     printf( "\x1B[%um", 33 )
#define TextFG_Blue()       printf( "\x1B[%um", 34 )
#define TextFG_Magenta()    printf( "\x1B[%um", 35 )
#define TextFG_Cyan()       printf( "\x1B[%um", 36 )
#define TextFG_White()      printf( "\x1B[%um", 37 )
#define TextBG_Black()      printf( "\x1B[%um", 40 )
#define TextBG_Red()        printf( "\x1B[%um", 41 )
#define TextBG_Green()      printf( "\x1B[%um", 42 )
#define TextBG_Yellow()     printf( "\x1B[%um", 43 )
#define TextBG_Blue()       printf( "\x1B[%um", 44 )
#define TextBG_Magenta()    printf( "\x1B[%um", 45 )
#define TextBG_Cyan()       printf( "\x1B[%um", 46 )
#define TextBG_White()      printf( "\x1B[%um", 47 )
#define TextColor(x)        printf( "\x1B[%um", (x) )
#define TextGotoXY(x,y)     printf( "\x1B[%u;%uH",(y),(x))
#define TextClearScreen()   printf( "\x1B[2J" )
#define getakey() getchar()


int miliseg, tecla;
int kup, kleft, kright, kdown;
int level = 0, frame;
int puntos, jugadas;
int last=0, best=0, pause=0;
int sound = 1;

int key_up = 'q';
int key_down = 'a';
int key_left = 'o';
int key_right = 'p';
int key_pause = 'm';

#define levels 10

int wait[levels] = { 35, 31, 29, 27, 25, 23,
                     21, 19, 16, 13 };
int colorfg[levels] = { 0, 0, 0, 0, 0, 
                        0, 0, 0, 0, 0 };
int colorbg[levels] = { 43, 46, 42, 45, 47,
                        43, 46, 42, 45, 47 };
int attrib_level[levels] = { 6, 5, 4, 3, 7,
                             6, 5, 4, 3, 7 };
int limite_nivel[levels] = { 25, 50, 75, 100, 150,
                             200, 250, 300, 350, 400 };

/*
0 black
1 blue
2 red
3 purple, or magenta
4 green
5 pale blue, or cyan
6 yellow
7 white
*/

int sleep(int secs);
int ticksleep(int ticks);
void Startup( int game );
void UpdateScore( int change_level );
void Game( void );
void DrawMenu( void );
void DrawCredits( void );
void ShowKeys( void );
void Show_a_key( char key );
void RedefineKeys( void );
char Redefine_a_key( void );
long start, end;
char GetKey( void );

#define CLICK()    if( sound ) { BEEP(50, 10); }
#define OPEN_SND() if( sound ) { BEEP(25, 3); }
#define DIE_SND()  if( sound ) { BEEP(1000, 20); }


//--- Main function ---------------------------------------------------
int main( void )
{
  unsigned char done=0;
  int counter;
  unsigned int nextup, nextmiddle, nextbottom;
  // Are we on the main menu? 0/1
  int on_menu, color, color2, i;
  
  BORDER(0);
  Startup( 1 );
  counter = color = 1;
  on_menu = 1;
  start = clock();
  
  DrawMenu();
  while(!done)
  {
     end = clock();

     if( on_menu )
     {
        tecla = GetKey();
        switch( tecla )
        {
           case '1':
               CLICK(); 
               Startup( 0 ); DrawBoard();
               Game();
               start = end = clock();
               if( puntos > best ) best = puntos;
               last = puntos;
               EraseBoard();
               ClearScreenBlock( 10, 9, 14, 8, 255 );
               TextGotoXY(15,10); printf(" GAME OVER!");
               TextGotoXY(15,12); printf(" Score: %d ", puntos );
               TextGotoXY(15,13); printf(" Level: %d ", level);
               TextGotoXY(15,14); printf(" Best:  %d ", best);
               WaitKey(); CLICK(); 
               UpdateScore( 1 ); DrawMenu();
               on_menu = 1;
               break;
           case '2':
               CLICK();
               RedefineKeys();
               on_menu = tecla = 0;
               break;
           case '3':
               CLICK();
               ShowKeys();
               on_menu = tecla = 0;
               break;
           case '4':
               sound = 1-sound;
               CLICK(); TextGotoXY(13,13);
               if( sound )        printf("4. SOUND: ON  ");
               else               printf("4. SOUND: OFF ");
               break;
           case '5':
               CLICK();
               DrawCredits();
               on_menu = tecla = 0;
               break;
         } // end switch

     } // end if
     else
     {
        if( GetKey() != 0 )
        { on_menu = 1; CLICK(); DrawMenu(); }
     }
     
     // menu color animation
     counter++;
     if( counter > 700 )
     {
        nextup = color;
        color++;
        if( color > 7 ) color = 1;
        DrawNextBlock( 27, 5, nextup, nextup, nextup );
        ClearScreenAttr( 10, 10, 2, 7, attributes[nextup] << 3 );
        if( --nextup == 0 ) nextup = 7;
        ClearScreenAttr( 12, 10, 2, 7, attributes[nextup] << 3 );
        if( --nextup == 0 ) nextup = 7;
        ClearScreenAttr( 14, 10, 2, 7, attributes[nextup] << 3 );
        if( --nextup == 0 ) nextup = 7;
        ClearScreenAttr( 16, 10, 2, 7, attributes[nextup] << 3 );
        if( --nextup == 0 ) nextup = 7;
        ClearScreenAttr( 18, 10, 2, 7, attributes[nextup] << 3 );
        if( --nextup == 0 ) nextup = 7;
        ClearScreenAttr( 20, 10, 3, 7, attributes[nextup] << 3 );
        counter = 0;
     }
  }
}


//---------------------------------------------------------------------
void Game( void )
{
  int xb, yb, oldpuntos;
  unsigned char done = 0, tmp;
  unsigned int up, middle, bottom;
  unsigned int nextup, nextmiddle, nextbottom;
  int first = 2;

  srand(clock());
  xb = initial_x; yb = initial_y;
  GenerateBlock( &nextup, &nextmiddle, &nextbottom );
  DrawNextBlock( 27, 5, nextup, nextmiddle, nextbottom);
  GenerateBlock( &up, &middle, &bottom );
  DrawBlock(xb, yb, up, middle, bottom, first );

  start = clock();

  while(!done)  // game loop
  {
    if( pause == 1 )                // Check for pause key
    {
       while( GetKey() != key_pause ) ;
       pause = 0;
    }
    
    // start our timer
    end = clock();

    // check keyboard
    tecla = GetKey();
    if( tecla == key_left )        kleft=1;
    else if( tecla == key_right )  kright=1;
    else if( tecla == key_up )     kup=1;
    else if( tecla == key_down )   kdown=1;
    else if( tecla == key_pause )  pause=1;
    // special keys: end game and raise/lower level
    //if( tecla == 'x' )   done =1;
    //if( tecla == 'r' && level < 4 ) level++;
    //if( tecla == 'e' && level > 0 ) level--;

    // Move left
    if( kleft && CanMoveLeft(xb,yb,first) )
    { 
       EraseBlock(xb,yb, first);
       xb--;
       DrawBlock(xb, yb, up, middle, bottom, first );
       kleft = kright = 0;
    }
    
    // Move right
    if( kright && CanMoveRight(xb,yb,first) )
    {  
       EraseBlock(xb,yb,first);
       xb++;
       DrawBlock(xb, yb, up, middle, bottom, first );
       kright = kleft = 0;
    }

    // Rotate piece
    if( kup ) 
    { 
       tmp = bottom; bottom = middle; middle = up; up = tmp; 
       kup = 0; 
       EraseBlock(xb,yb,first);
       DrawBlock(xb, yb, up, middle, bottom, first );
    }

    // drop piece
    if( kdown ) 
    { 
       kdown = 0; 
       EraseBlock(xb,yb,first);
       tmp = FindSlot(xb);

       // first is the stupid variable for the stupid system used
       // to determine if the jewel has appeared entirely
       // first=2 -> 1/3 jewel on the screen, =1 -> 2/3 jewels, =0 -> entire.
       if( tmp == 0 )      first = 2;
       else if( tmp == 1 ) first = 1;
       else                first = 0;
       if( first == 0 )    tmp-=2;
       DrawBlock( xb, tmp, up, middle, bottom, first );
       yb = tmp;
    }
     
    if( end >= start+wait[level] )
    {
       if( CanFall(xb, yb, first ) )
       {
          EraseBlock(xb,yb,first);
          if( first > 0 ) first--;
          else            yb++;
          DrawBlock(xb, yb, up, middle, bottom, first );
       }
       else
       {
          OPEN_SND();
          
          // check if we have died!
          InsertBlock( xb, yb, up, middle, bottom, first );
          // if it's an special block, replace all the blocks of
          // a given color by the special block, to remove it.
          if( bottom == special && yb < board_height-3) 
             ReplaceBySpecial(xb, yb+3);
          
          RedrawBoard();
          if( bottom == special ) BlocksDown();
          up = nextup; middle = nextmiddle; bottom = nextbottom;
          GenerateBlock( &nextup, &nextmiddle, &nextbottom );
          yb = initial_y; xb = initial_x;
          oldpuntos = 0;
          jugadas = CheckBoard();
          RedrawBoard();
          BlocksDown();
          if( jugadas > 0 ) { CLICK(); }
          puntos += jugadas;
          do 
          {
            jugadas = CheckBoard();
            RedrawBoard();
            BlocksDown();
            oldpuntos += jugadas;
            if( jugadas > 0 ) { CLICK(); }
          }
          while (jugadas != 0);

          puntos += (oldpuntos << 1);
          RedrawBoard();
          
          if( puntos >= limite_nivel[level] && level < levels-1 )
          {
             level++; UpdateScore( 1 );
             DrawNextBlock( 27, 5, nextup, nextmiddle, nextbottom);
          }
          else UpdateScore( 0 );
          DrawNextBlock( 27, 5, nextup, nextmiddle, nextbottom);
          
          if( FindSlot(xb) == 666 ) { end=start; done=1; }
          else
          {
             first = 2;
             DrawBlock(xb, yb, up, middle, bottom, first );
             kleft = kright = kup = kdown = 0;
          }
       }
       start = clock();
       frame = 1-frame;
    }
  }
  DIE_SND();
  
}


//---------------------------------------------------------------------
char GetKey( void )
{  return( getk() ); }


//---------------------------------------------------------------------
void DrawMenu( void )
{

   ClearScreenBlock( 10, 9, 14, 8, 255 );
   TextGotoXY(13,8);  printf("  ZX COLUMNS  ");
   ClearScreenAttr( 10, 8, 1, 1, 0 );
   TextGotoXY(13,10); printf("1. START GAME ");
   TextGotoXY(13,11); printf("2. DEFINE KEYS ");
   TextGotoXY(13,12); printf("3. SHOW KEYS ");
   TextGotoXY(13,13);
   if( sound )        printf("4. SOUND: ON  ");
   else               printf("4. SOUND: OFF ");
   TextGotoXY(13,14); printf("5. CREDITS ");
}


//---------------------------------------------------------------------
void DrawCredits( void )
{
   
   ClearScreenBlock( 10, 9, 14, 8, 255 );
   TextGotoXY(13,10); printf("Code:  NoP     ");
   TextGotoXY(13,11); printf("Gfx:   Matriax ");
   TextGotoXY(13,13); printf("  THANKS TO    ");
   TextGotoXY(13,14); printf(" Fede, Miguel, ");
   TextGotoXY(13,15); printf(" Siew, E.C.S.S.");
   TextGotoXY(13,16); printf("& Jesus Ramos  ");
   WaitKey();
}


//---------------------------------------------------------------------
void ShowKeys( void )
{
   
   ClearScreenBlock( 10, 9, 14, 8, 255 );
   TextGotoXY(13,10); printf(" LEFT   :  "); Show_a_key(key_left);
   TextGotoXY(13,11); printf(" RIGHT  :  "); Show_a_key(key_right);
   TextGotoXY(13,12); printf(" ROTATE :  "); Show_a_key(key_up);
   TextGotoXY(13,13); printf(" DROP   :  "); Show_a_key(key_down);
   TextGotoXY(13,14); printf(" PAUSE  :  "); Show_a_key(key_pause);
   TextGotoXY(13,16); printf(" PRESS ANY KEY ");
   WaitKey();
}


//---------------------------------------------------------------------
void Show_a_key( char key )
{
   if( key == ' ' )       printf("SPC ");
   else if( key == '\n' ) printf("ENT ");
   else                   printf("%c ", key );
}


//---------------------------------------------------------------------
void RedefineKeys( void )
{
   ClearScreenBlock( 10, 9, 14, 8, 255 );

   TextGotoXY(13,10); printf(" LEFT   :  "); 
   key_left = Redefine_a_key(); CLICK(); Show_a_key(key_left);
   
   TextGotoXY(13,11); printf(" RIGHT  :  "); 
   key_right = Redefine_a_key(); CLICK(); Show_a_key(key_right);
   
   TextGotoXY(13,12); printf(" ROTATE :  "); 
   key_up = Redefine_a_key(); CLICK(); Show_a_key(key_up);
   
   TextGotoXY(13,13); printf(" DROP   :  ");
   key_down = Redefine_a_key(); CLICK(); Show_a_key(key_down);
   
   TextGotoXY(13,14); printf(" PAUSE  :  ");
   key_pause = Redefine_a_key(); CLICK(); Show_a_key(key_pause); 

   TextGotoXY(13,16); printf(" PRESS ANY KEY ");
   WaitKey();
}


//---------------------------------------------------------------------
char Redefine_a_key( void )
{
   char done = 0, tecla;
   
   while(!done)
   {
     tecla = WaitKey();
     if( tecla == '\n' || tecla == ' ' ) return(tecla);
     else if( tecla>='0' && tecla <='9' ) return(tecla);
     else if( tecla>='a' && tecla <='z' ) return(tecla);
     else if( tecla>='A' && tecla <='Z' ) return(tecla);
   }
   return(' ');
}


//---------------------------------------------------------------------
void UpdateScore( int change_level )
{
   TextColor(colorbg[level]);
   TextGotoXY(7,5); 
   printf("%d", puntos);
   if( change_level )
   {
      SetSpriteAttrib( 0,  0, 10, 24, attrib_level[level]<<3 );
      SetSpriteAttrib( 24, 0, (32-24), 24, attrib_level[level]<<3 );
      TextGotoXY(7,6); printf("%d", level);
      TextGotoXY(7,8); printf("%d", last);
      TextGotoXY(7,9); printf("%d", best);
   }
}


//---------------------------------------------------------------------
// game = 1 -> Startup the game itself
// game = 0 -> Startup just a new game
void Startup( int game )
{
   int y;
   
   level =  pause =  puntos =  jugadas = 0;

   if( game )
   {
      InitGFX();
      ClearScreenAttrib( VRAM, 0 );
      DrawScreenNOATTR( fondo );
   }
   
   TextColor(colorbg[level]); TextReverse();
   if( !game ) ClearScreenBlock( 5, 5, 3, 5, 255 );
   ClearBoard();
   Grafico = joya2;
   UpdateScore( 1 );
   DrawBoard();
   kup = kleft = kright = kdown = 0;
}


//---------------------------------------------------------------------
int sleep(int secs)
{
  long start = clock();
  secs *= CLOCKS_PER_SEC;
  while ( (clock() - start) < secs )
             ;
   return 0;
}


//---------------------------------------------------------------------
int ticksleep(int ticks)
{
  long start = clock();
  while ( (clock() - start) < ticks )
             ;
   return 0;
}

