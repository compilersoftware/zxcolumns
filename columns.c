/*
 
 Core routines for columns game.

 ---------------------------------------------------------------------

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

 ---------------------------------------------------------------------
 
  en el centro el tablero (ocupa 1 tercio exacto de la pantalla)
  a la izquierda la pieza siguiente, la puntuacion, el nivel,
  las joyas que has hecho, a la derecha el logotipo.
  Tablero de 7x12
 
   0 = hueco (negro)
   1 = azul
   2 = rojo
   3 = morado
   4 = verde
   5 = cyan
   6 = amarillo
   7 = blanco = especial
   8 = (o mayor) bloque marcado para borrar con animacion
         La animacion consiste en dibujar en los siguientes 7
         fotogramas cada uno de los bloques anteriores. El octavo
         fotograma es negro y el noveno es que cae hacia abajo.
         color_actual = valor - 7;
         if( valor = 15 ) -> pintar negro (poner los > 8 a cero)
         if( valor = 16 ) -> hacer caer los bloques si bloque = 0;
   96x192    Piezas de 16x12 pixels centradas = 12x12 (bordes
             derechos e izquierdos con fondo).
*/

//--- Variables and defines -------------------------------------------

#define  board_width     7
#define  board_height   12
#define  board_size     board_width * board_height

#define  empty           0
#define  special         7
#define  deleteit        8
#define  num_blocks      6
#define  x_board        10
#define  special_chance 96

#define incr 31
#define decr 31

unsigned char board[ board_size ];

// Optimize the (y*board_width) multiplication
#define YBYWIDTH(y)  (  ((y)<<2) + ((y)<<1) + (y))
//#define YBYWIDTH(y)  (  ((y)<<2) + ((y)<<1) )
//#define YBYWIDTH(y)  ( (y)*7 )

// Optimized: n*6 = n*4 + n*2 = n<<2 + n<<1
#define GetBoard( x, y )       \
   (board[YBYWIDTH(y)+(x)])
   
#define SetBoard( x, y, val )  \
   (board[YBYWIDTH(y)+(x)] = (val))

#define SetTMPBoard( x, y, val )  \
   (tmpboard[YBYWIDTH(y)+(x)] = (val))
   
#define GetBoardClip( x, y )       \
   if( (x) > 0 && (x) < board_width && \
       (y) > 0 && (y) < board_height ) \
   (board[YBYWIDTH(y)+(x)])
   
#define SetBoardClip( x, y, val )  \
   if( (x) > 0 && (x) < board_width && \
       (y) > 0 && (y) < board_height ) \
   (board[YBYWIDTH(y)+(x)] = (val))


                         
//--- Functions -------------------------------------------------------
void ClearBoard( void );
int  CheckBoard( void );
void BlocksDown( void );
int  FindSlot( int x );

void DrawBoard( void );
void RedrawBoard( void );
void EraseBoard( void );
void DrawBlock( int x, int y, unsigned char up,
                unsigned char middle, unsigned char bottom, char first );
void DrawNextBlock( int x, int y, unsigned char up, 
                unsigned char middle, unsigned char bottom );
void EraseBlock( int x, int y, char first );
int  CanFall( int x, int y, char first );
void InsertBlock( int x, int y, unsigned char up, 
                  unsigned char middle, unsigned char bottom, char first );
void GenerateBlock( unsigned int *up, unsigned int *middle,
                    unsigned int *bottom );
int  CanMoveLeft( int x, int y, char first );
int  CanMoveRight( int x, int y, char first );


//--- Clear the entie board ------------------------------------------
void GenerateBlock( unsigned int *up, unsigned int *middle, 
                    unsigned int *bottom )
{
   int tmp;
   tmp = (unsigned char) (abs(rand()) % 100 )+1;
   if( tmp >= special_chance )
   {
      *up = *middle = *bottom = 7;
   }
   else
   {
      *up =     (unsigned char) (abs(rand()) % num_blocks)+1;
      *middle = (unsigned char) (abs(rand()) % num_blocks)+1;
      *bottom = (unsigned char) (abs(rand()) % num_blocks)+1;
   }
}


//--- Clear the entire board ------------------------------------------
#define ClearBoard()   memset(board, (unsigned char) empty, board_size);


//--- Insert the block in the array -----------------------------------
void InsertBlock( int x, int y, unsigned char up, 
                  unsigned char middle, unsigned char bottom, char first)
{ 

  if( first == 0 )
  {
     SetBoard( x, y, up );
     SetBoard( x, y+1, middle );
     SetBoard( x, y+2, bottom );
  }
  else if( first == 1 )
  {
     SetBoard( x, y, middle );
     SetBoard( x, y+1, bottom );
  }
  else if( first == 2 )
  {
     SetBoard( x, y, bottom );
  }

}


//--- Can fall: 1: current block can fall, 0: can't -------------------
int CanFall( int x, int y, char first )
{
  if( (y-first)+3 > board_height-1 )  return(0);
  if( GetBoard(x, (y-first)+3) != 0 ) return(0);
  return(1);
}


//--- Can move left ---------------------------------------------------
int CanMoveLeft( int x, int y, char first )
{
  if( x <= 0 )                        return(0);
  if( GetBoard(x-1, y-first+2) != 0 ) return(0);
  return(1);
}


//--- Can move right --------------------------------------------------
int CanMoveRight( int x, int y, char first )
{
  if( x >= board_width-1 )            return(0);
  if( GetBoard(x+1, y-first+2) != 0 ) return(0);
  return(1);
}


//--- Draw the entire board -------------------------------------------
void DrawBoard( void )
{
  int x, y;
  
  // first clear the board to the given color (black)
  SetSpriteAttrib( x_board, 0, board_width<<1 , board_height<<1, 0 );

  for( y=0; y<board_height; y++ )
    for( x=0; x<board_width; x++ )
       DrawSpriteBlockNOATTR( x_board+(x<<1), (y<<1), 2, 2, Grafico );

  DrawSpriteBlockNOATTR( 27, 5, 2, 2, Grafico );
  DrawSpriteBlockNOATTR( 27, 7, 2, 2, Grafico );
  DrawSpriteBlockNOATTR( 27, 9, 2, 2, Grafico );
}

//--- Clear the entire board ------------------------------------------
void ReplaceBySpecial( int xb, int yb )
{
  int x;
  unsigned char tmp;
  tmp = GetBoard( xb, yb );
  for( x=0; x<board_size; x++ ) 
     if( board[x] == tmp ) board[x] = special;
}


//--- Find an empty slot ----------------------------------------------
// (returns 666 if there is not empty slot)
int FindSlot( int x )
{
  int y;

  for( y=board_height-1; y>=0; y-- )
     if( GetBoard(x, y) == 0 ) return(y);

  return(666);
}


//--- Draw the entire board -------------------------------------------
void RedrawBoard( void )
{
  int x, y, donde;
  unsigned char attr;
  unsigned char *memoffset;
  
  
  x = x_board;
  y = 0;
   
  // memoffset = 0x4000 + 61414 + (y*32*2) + (x*2)
  memoffset = (unsigned char *) (0x4000 + 6144 + (y << 6) + (x)) ;
   
  for( y=0; y<board_height; y++ )
  {
    donde = YBYWIDTH(y);
   
    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;      memoffset -= decr;

    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;      memoffset -= decr;
    
    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;      memoffset -= decr;

    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;      memoffset -= decr;

    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;      memoffset -= decr;
    
    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;      memoffset -= decr;
    
    attr = (unsigned char) attributes[board[donde++]];
    *memoffset = attr + 64; memoffset++;
    *memoffset = attr;      memoffset += incr;
    *memoffset = attr;      memoffset++;
    *memoffset = attr;   
   
    memoffset += 32-((board_width << 1)-1);
  }        
  
}

//--- Erase the entire board -------------------------------------------
void EraseBoard( void )
{
  int x, y;
  unsigned char *memoffset;
  
  x = x_board; y = 0;
   
  memoffset = (unsigned char *) (0x4000 + 6144 + (y << 6) + (x)) ;
  for( y=0; y<board_height<<1; y++ )
  {
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0; memoffset++;
    *memoffset = 0;
    memoffset += 32-((board_width << 1)-1);
  }        
  
}

//--- Draw the piece -----------------------------------------------
void DrawBlock( int x, int y, unsigned char up, unsigned char middle,
                unsigned char bottom, char first )
{
  unsigned char attr;
  unsigned char *memoffset;
  
  memoffset = (unsigned char *) (0x4000 + 6144 + (y << 6) + (x << 1) + x_board) ;

  // first = 2 when we are drawing just a single block (yb=-2)
  if( first == 2 )
  {
      attr = attributes[bottom];
      
      // bottom block : add bright attribute 
      *memoffset = attr + 64; memoffset++;
      *memoffset = attr;      memoffset += incr;
      *memoffset = attr;      memoffset++;
      *memoffset = attr;
  }
  
  // first = 1 when we are drawing just 2 blocks (yb=-1)
  else if( first == 1 )
  {
      attr = attributes[middle];
      // upper-left block : add bright attribute 
      *memoffset = attr + 64; memoffset++;
      *memoffset = attr;      memoffset += incr;
      *memoffset = attr;      memoffset++;
      *memoffset = attr;      memoffset += incr;
     
      attr = attributes[bottom];
      // upper-left block : add bright attribute 
      *memoffset = attr + 64; memoffset++;
      *memoffset = attr;      memoffset += incr;
      *memoffset = attr;      memoffset++;
      *memoffset = attr;

  }
  // all 3 blocks are displayed
  else
  {
      attr = attributes[up];
      // upper-left block : add bright attribute 
      *memoffset = attr + 64; memoffset++;
      *memoffset = attr;      memoffset += incr;
      *memoffset = attr;      memoffset++;
      *memoffset = attr;      memoffset += incr;
    
      attr = attributes[middle];
      // upper-left block : add bright attribute 
      *memoffset = attr + 64; memoffset++;
      *memoffset = attr;      memoffset += incr;
      *memoffset = attr;      memoffset++;
      *memoffset = attr;      memoffset += incr;
    
      attr = attributes[bottom];
      // upper-left block : add bright attribute 
      *memoffset = attr + 64; memoffset++;
      *memoffset = attr;      memoffset += incr;
      *memoffset = attr;      memoffset++;
      *memoffset = attr;
  } // else
}


//--- Draw the piece -----------------------------------------------
void DrawNextBlock( int x, int y, unsigned char up, unsigned char middle, unsigned char bottom )
{
  unsigned char *memoffset;
  unsigned char attr;
  
   // memoffset = 0x4000 + 61414 + (y*32) + x
   memoffset = (unsigned char *) (0x4000 + 6144 + (y*32) + (x)) ;
   
   attr = attributes[up];
   // upper-left block : add bright attribute 
   *memoffset = attr + 64; memoffset++;
   *memoffset = attr;      memoffset += incr;
   *memoffset = attr;      memoffset++;
   *memoffset = attr;      memoffset += incr;
 
   attr = attributes[middle];
   // upper-left block : add bright attribute 
   *memoffset = attr + 64; memoffset++;
   *memoffset = attr;      memoffset += incr;
   *memoffset = attr;      memoffset++;
   *memoffset = attr;      memoffset += incr;
  
   attr = attributes[bottom];
   // upper-left block : add bright attribute 
   *memoffset = attr + 64; memoffset++;
   *memoffset = attr;      memoffset += incr;
   *memoffset = attr;      memoffset++;
   *memoffset = attr;
}


//--- Erase the piece -----------------------------------------------
void EraseBlock( int x, int y, char first )
{
  unsigned char attr;
  unsigned char *memoffset;
  
  memoffset = (unsigned char *) (0x4000 + 6144 + (y << 6) + (x << 1) + x_board) ;
  attr = 0;
  // erase just first block
  if( first == 2 )
  {
      // memoffset = 0x4000 + 61414 + (y*32*2) + (x*2)
      //attr = (unsigned char) attributes[GetBoard(x,y+2)];
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
      *memoffset = attr; memoffset++;
      *memoffset = attr;
 
  }
  // erase two blocks
  else if( first == 1 )
  {
      //attr = (unsigned char) attributes[GetBoard(x,y+1)];
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
     
      //attr = (unsigned char) attributes[GetBoard(x,y+2)];
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
      *memoffset = attr; memoffset++;
      *memoffset = attr; 
  }
  // erase three blocks
  else
  {
   
      //attr = (unsigned char) attributes[GetBoard(x,y)];
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
 
      //attr = (unsigned char) attributes[GetBoard(x,y+1)];
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
     
      //attr = (unsigned char) attributes[GetBoard(x,y+2)];
      *memoffset = attr; memoffset++;
      *memoffset = attr; memoffset += incr;
      *memoffset = attr; memoffset++;
      *memoffset = attr;
  }
}


//--- Make the blocks fall --------------------------------------------
void BlocksDown( void )
{
  int mx, my, mf;
  unsigned char dato;

  
  //--- fall version: all blocks fall at a time ---
  for( my=board_height-1; my>0; my-- )
   for( mx=0; mx<board_width; mx++ )
   {
     dato = GetBoard(mx, my);
     if( dato == empty || dato == special )
     {
       for( mf=my-1; mf>=0; mf--)
       {
          dato = GetBoard(mx,mf);
          if( dato != empty && dato != special) break;
       }
       SetBoard( mx, mf, empty );
       SetBoard( mx, my, dato );
     }
   }
    
}



//--- Check the entire board for valid combinations -------------------
//--- it also checks for "deleteit" blocks and updates them -------------
//--- returns number of games done * 8 --------------------------------
int CheckBoard( void )
{
  int x, y;
  unsigned char dato, retcode;
  unsigned char tmpboard[ board_size ];

  // make a copy of the board to work in
  memcpy( tmpboard, board, board_size );
  
  retcode = 0;
  
  for( y=0; y<board_height; y++ )
   for( x=0; x<board_width; x++ )
   {

      // get current data and ...
      dato = GetBoard( x, y );
      
      //----------------------------------------------
      // Check for valid combinations
      //----------------------------------------------
      // we start on 2 because the UP and LEFT tests will test the 0 &
      // 1 blocks when testing the block number 2 for a valid combination
      // Up-left and up-right tests will detect down-left & down-right
      if ( dato < deleteit && dato != special && dato != empty )
      {
         if( y>=2 )
         {
           // check for up combinations
           if( dato == GetBoard( x, y-1) &&
               dato == GetBoard( x, y-2 ) )
           {
              SetTMPBoard( x, y, special );
              SetTMPBoard( x, y-1, special );
              SetTMPBoard( x, y-2, special );
              retcode++;
           }
           if( x>= 2 )
           {
              // check for up - left combinations
              if( dato == GetBoard( x-1, y-1) &&
                  dato == GetBoard( x-2, y-2 ) )
              {
                SetTMPBoard( x, y, special );
                SetTMPBoard( x-1, y-1, special );
                SetTMPBoard( x-2, y-2, special );
                retcode++;
              }
           }

         }
         if( x>= 2)
         {
           // check for left combinations
           if( dato == GetBoard( x-1, y) &&
               dato == GetBoard( x-2, y ) )
           {
              SetTMPBoard( x, y, special );
              SetTMPBoard( x-1, y, special );
              SetTMPBoard( x-2, y, special );
              retcode++;
           }
         }
         if( y>=2 && x <= board_width-3 )
         {
            // check for up - right combinations
            if( dato == GetBoard( x+1, y-1) &&
                dato == GetBoard( x+2, y-2 ) )
            {
              SetTMPBoard( x, y, special );
              SetTMPBoard( x+1, y-1, special );
              SetTMPBoard( x+2, y-2, special );
              retcode++;
            }
         }
   
     }
     //----------------------------------------------
     if( dato == 15 )
     {
        SetTMPBoard( x, y, 0 );
     }
     else if( dato >= deleteit )
     {
        SetTMPBoard( x, y, dato+1 );
     }
  }

  // now copy the temp board to the real one.
  memcpy( board, tmpboard, board_size );

  return(retcode);
}
  

