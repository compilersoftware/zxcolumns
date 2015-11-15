/*

 Gfx functions

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

*/

int scan_convert[192] = {
   0,8,16,24,32,40,48,56,
   1,9,17,25,33,41,49,57,
   2,10,18,26,34,42,50,58,
   3,11,19,27,35,43,51,59,
   4,12,20,28,36,44,52,60,
   5,13,21,29,37,45,53,61,
   6,14,22,30,38,46,54,62,
   7,15,23,31,39,47,55,63,
                   
   64,72,80,88,96,104,112,120,
   65,73,81,89,97,105,113,121,
   66,74,82,90,98,106,114,122,
   67,75,83,91,99,107,115,123,
   68,76,84,92,100,108,116,124,
   69,77,85,93,101,109,117,125,
   70,78,86,94,102,110,118,126,
   71,79,87,95,103,111,119,127,

   128,136,144,152,160,168,176,184,
   129,137,145,153,161,169,177,185,
   130,138,146,154,162,170,178,186,
   131,139,147,155,163,171,179,187,
   132,140,148,156,164,172,180,188,
   133,141,149,157,165,173,181,189,
   134,142,150,158,166,174,182,190,
   135,143,151,159,167,175,183,191
};

//#define GetRowAddress(row) ((scan_convert[(row)]*32)+0x4000)
#define GetRowAddress(row) (scan_convert[(row)])
#define SCREENSIZE 6912
unsigned char *VRAM;
void InitGFX( void );

int DrawSpriteBlock( unsigned char x, unsigned char y, 
                     unsigned char width, unsigned char height, 
                     unsigned char *sprite, unsigned char *attrib );

int DrawSpriteAttrib( unsigned char x, unsigned char y, 
                     unsigned char width, unsigned char height, 
                     unsigned char *attrib );
int GetSpriteBlock(  unsigned char x, unsigned char y, 
                     unsigned char width, unsigned char height, 
                     unsigned char *sprite, unsigned char *attrib );

int DrawSpriteBlockNOATTR( unsigned char x, unsigned char y, 
                           unsigned char width, unsigned char height, 
                           unsigned char *sprite );
void CopyScreen( unsigned char *org, unsigned char *dst, int size );
void ClearScreen( unsigned char *dest, int value, int color );
void ClearScreenAttrib( unsigned char *dest, int color );
int DrawScreen( unsigned char *sprite, unsigned char *attrib );
int DrawScreenNOATTR( unsigned char *sprite );
void ClearScreenBlock( int x, int y, int w, int h, int value );
void ClearScreenAttr( int x, int y, int width, int height, int value );
void BORDER( unsigned char value );
void BEEP(unsigned int cycles, unsigned int tone);

unsigned char my_tmp_border;

//---------------------------------------------------------------------
// Init GFX stuff...
//---------------------------------------------------------------------
void InitGFX( void )
{
  int x;
  VRAM = 0x4000;
  
  for(x=0; x<192; x++)
    scan_convert[x] = (scan_convert[x] * 32) + (int) VRAM;
}


//=== BLOCK SPRITES ===================================================
//
// Create optimized functions for 1x2, 2x1, 2x2 and 4x4 sprites
// 

//---------------------------------------------------------------------
// DrawSpriteBlock();   Draw (w,h) sprites in unsigned character blocks (x,y).
//
// x,y          = Character block x & y (0-31 and 0-23)
// width,height = Sprite size in unsigned character blocks
// sprite       = Sprite to draw in a linear array
// attrib       = attributes in a linear array
//---------------------------------------------------------------------
int DrawSpriteBlock( unsigned char x, unsigned char y, 
                     unsigned char width, unsigned char height, 
                     unsigned char *sprite, unsigned char *attrib )
{
  int i, j;
  unsigned char *sproff;           /* offset into the sprite */
  unsigned char *memoffset;

  height = height<<3; 
  y = y<<3;
  
  sproff = (unsigned char *) &sprite[0];
  for( j=0; j<height; j++ )
  {
     /* get the screen offset and draw the scanlines */
     memoffset = (unsigned char *) GetRowAddress(y+j) +x;
     memcpy( memoffset, sproff, width );
     sproff+=width;
  }

  // now we put the attributes on the videomemory
  y >> 3;
  height >> 3;
  sproff = (unsigned char *) &attrib[0];
  memoffset = (unsigned char *) ( 22528 + (y<<5) + x) ;
  for(j=0; j<height; j++)
  {
     memcpy( memoffset, sproff, width );
     sproff+=width;
     memoffset += (32-width);
  }
  
}


//--------------------------------------------------------------
int DrawSpriteBlockNOATTR( unsigned char x, unsigned char y, 
                     unsigned char width, unsigned char height, 
                     unsigned char *sprite )
{
  int i, j;           /* offset into the sprite */
  unsigned char *memoffset;
  unsigned char *sproff;

  sproff = (unsigned char *) &sprite[0];
  y = y<<3;
  height = height<<3;

  for( j=0; j<height; j++ )
  {
     /* get the screen offset and draw the scanlines */
     memoffset = (unsigned char *) GetRowAddress(y+j) +x;
     memcpy( memoffset, sproff, width );
     sproff+=width;
  }
 
}

//------------------------------------------------------------
int DrawSpriteAttrib( unsigned char x, unsigned char y, 
                      unsigned char width, unsigned char height, 
                      unsigned char *attrib )
{
  int i, j; 
  unsigned char *sproff;           /* offset into the sprite attributes*/
  unsigned char *memoffset;

  sproff = (unsigned char *) &attrib[0];
  memoffset = (unsigned char *) (0x4000 + 6144 + (y<<5) + x) ;
  
  for(j=0; j<height; j++)
  {
     memcpy( memoffset, sproff, width );
     sproff+=width;
     memoffset += (32-width);
  }
}


// clear the sprite with a given attribute
//------------------------------------------------------------
int SetSpriteAttrib( unsigned char x, unsigned char y, 
                      unsigned char width, unsigned char height, 
                      unsigned char attrib )
{
  int i, j;           /* offset into the sprite */
  unsigned char *memoffset;

  memoffset = (unsigned char *) (0x4000 + 6144 + (y<<5) + (x)) ;
  for(j=0; j<height; j++)
  {
     memset( memoffset, attrib, width );
     memoffset += 32;
     //for(i=0;i<width;i++) *memoffset++ = attrib
     //memoffset += (32-width);
  }
}

//-----------------------------------------------------------
int DrawScreen( unsigned char *sprite, unsigned char *attrib )
{

  int x, y;
  unsigned char *sproff;
  unsigned char *memoffset;

  // get the screen offset and draw the scanlines 
  sproff    = (unsigned char *) &sprite[0];

  for( y=0; y<192; y++ )
  {
      memoffset = (unsigned char *) GetRowAddress(y);
      memcpy( memoffset, sproff, 32 );
      sproff+=32;
  }

  // now we put the attributes on the videomemory
  memoffset = (unsigned char *) (0x4000 + 6144);
  memcpy( memoffset, attrib, 768 );
}


//-----------------------------------------------------------
int DrawScreenNOATTR( unsigned char *sprite )
{
  int x, y;
  unsigned char *sproff;
  unsigned char *memoffset;

  sproff    = (unsigned char *) &sprite[0];
  
  for( y=0; y<192; y++ )
  {
      memoffset = (unsigned char *) GetRowAddress(y);
      memcpy( memoffset, sproff, 32 );
      sproff+=32;
  }
}



//---------------------------------------------------------------------
// GetSpriteBlock();   Get (w,h) sprites in unsigned character blocks (x,y).
//
// x,y          = Character block x & y (0-31 and 0-23)
// width,height = Sprite size in unsigned character blocks
// sprite       = Where to get the sprite
// attrib       = Where to store the attributes
//---------------------------------------------------------------------
int GetSpriteBlock(  unsigned char x, unsigned char y, 
                     unsigned char width, unsigned char height, 
                     unsigned char *sprite, unsigned char *attrib )
{
  int i, j;
  unsigned char *sproff;           /* offset into the sprite */
  unsigned char *memoffset;

  height = height<<3; 
  y = y<<3;

  sproff = (unsigned char *) &sprite[0];
  
  for( j=0; j<height; j++ )
  {
     /* get the screen offset and draw the scanlines */
     memoffset = (unsigned char *) GetRowAddress(y+j) +x;
     memcpy( sproff, memoffset, width );
     sproff+=width;
  }

  // now we put the attributes on the videomemory
  y = y>>3;
  height = height>>3;
  
  memoffset = (unsigned char *) (0x4000 + 6144 + (y*32) + x) ;
  sproff = (unsigned char *) &attrib[0];
  for(j=0; j<height; j++)
  {
     memcpy( sproff, memoffset, width );
     memoffset += 32;
  }
  
}


//=== SCREEN FUNCTIONS ================================================
//
//

// Copy Memory Screen
void CopyScreen( unsigned char *orig, unsigned char *dest, int size )
{
  #asm
           ; take the 3 parameters from stack
           ld hl, 2
           add hl, sp
           ld c, (hl)
           inc hl
           ld b, (hl)  ; bc=size
           ld a, c
           or b
           ret z       ; check c!=0
           inc hl
           
           ld e, (hl)
           inc hl
           ld d, (hl)  ; DE = orig
           inc hl
           
           ld a, (hl)
           inc hl
           ld h, (hl)
           ld l, a     ; HL = dest

           ldir
  #endasm 
}


// Clear Memory Screen
void ClearScreen( unsigned char *dest, int value, int color )
{
  memset( (unsigned char *) VRAM + 6144, color, 768 );
  memset( (unsigned char *) VRAM, value, 192*32 );
}


// Clear Memory Screen Block
// x,y in blocks
// width, height in blocks
void ClearScreenBlock( int x, int y, int width, int height, int value )
{
  int j;
  unsigned char *memoffset;

  y = y<<3;
  height = height<<3;

  for( j=0; j<height; j++ )
  {
     // get the screen offset and draw the scanlines
     memoffset = (unsigned char *) GetRowAddress(y+j) +x;
     memset( memoffset, value, width);
  }

  /*
  // now we put the attributes on the videomemory
  y = y<<3;
  height = height << 3;
  
  memoffset = (unsigned char *) (0x4000 + 6144 + (y<<5) + x) ;
  for(j=0; j<height; j++)
  {
     memset( memoffset, color, width);
     memoffset += 32;
  }
  */
 
}

// Clear Memory Screen Attribute Block
// x,y in blocks
// width, height in blocks
void ClearScreenAttr( int x, int y, int width, int height, int value )
{
  int j;
  unsigned char *memoffset;

  // now we put the attributes on the videomemory
  
  memoffset = (unsigned char *) (0x4000 + 6144 + (y<<5) + x) ;
  for(j=0; j<height; j++)
  {
     memset( memoffset, value, width );
     memoffset += 32;
  }
 
}


// Clear Memory Screen attributes
void ClearScreenAttrib( unsigned char *dest, int color )
{
  memset( (unsigned char *) VRAM + 6144, color, 768 );
}


//---------------------------------------------------------------------
// Set Border
//---------------------------------------------------------------------
void BORDER( unsigned char value )
{
   my_tmp_border = value<<3;
#asm
   ld hl, 2
   add hl, sp
   ld a, (hl)
   ld c, 254
   out (c), a
   ld hl, 23624
   ld a, (_my_tmp_border)
   ld (hl), a
#endasm
}


//---------------------------------------------------------------------
void BEEP(unsigned int cycles, unsigned int tone)
{
#asm
   pop bc      ; bc = return address
   pop hl      ; hl = tone
   pop de      ; de = cycles
   push de
   push hl
   push bc
   jp 0x03b5   ; jump to ROM beep
#endasm
}

