/* <ctype.h> replacement macros.

   Copyright (C) 2000-2022 Free Software Foundation, Inc.
   Contributed by Zack Weinberg <zackw@stanford.edu>.

This file is part of the libiberty library.
Libiberty is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

Libiberty is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with libiberty; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
Boston, MA 02110-1301, USA.  */

/*

@defvr Extension HOST_CHARSET
This macro indicates the basic character set and encoding used by the
host: more precisely, the encoding used for character constants in
preprocessor @samp{#if} statements (the C "execution character set").
It is defined by @file{safe-ctype.h}, and will be an integer constant
with one of the following values:

@ftable @code
@item HOST_CHARSET_UNKNOWN
The host character set is unknown - that is, not one of the next two
possibilities.

@item HOST_CHARSET_ASCII
The host character set is ASCII.

@item HOST_CHARSET_EBCDIC
The host character set is some variant of EBCDIC.  (Only one of the
nineteen EBCDIC varying characters is tested; exercise caution.)
@end ftable
@end defvr

@deffn  Extension ISALPHA  (@var{c})
@deffnx Extension ISALNUM  (@var{c})
@deffnx Extension ISBLANK  (@var{c})
@deffnx Extension ISCNTRL  (@var{c})
@deffnx Extension ISDIGIT  (@var{c})
@deffnx Extension ISGRAPH  (@var{c})
@deffnx Extension ISLOWER  (@var{c})
@deffnx Extension ISPRINT  (@var{c})
@deffnx Extension ISPUNCT  (@var{c})
@deffnx Extension ISSPACE  (@var{c})
@deffnx Extension ISUPPER  (@var{c})
@deffnx Extension ISXDIGIT (@var{c})

These twelve macros are defined by @file{safe-ctype.h}.  Each has the
same meaning as the corresponding macro (with name in lowercase)
defined by the standard header @file{ctype.h}.  For example,
@code{ISALPHA} returns true for alphabetic characters and false for
others.  However, there are two differences between these macros and
those provided by @file{ctype.h}:

@itemize @bullet
@item These macros are guaranteed to have well-defined behavior for all 
values representable by @code{signed char} and @code{unsigned char}, and
for @code{EOF}.

@item These macros ignore the current locale; they are true for these
fixed sets of characters:
@multitable {@code{XDIGIT}} {yada yada yada yada yada yada yada yada}
@item @code{ALPHA}  @tab @kbd{A-Za-z}
@item @code{ALNUM}  @tab @kbd{A-Za-z0-9}
@item @code{BLANK}  @tab @kbd{space tab}
@item @code{CNTRL}  @tab @code{!PRINT}
@item @code{DIGIT}  @tab @kbd{0-9}
@item @code{GRAPH}  @tab @code{ALNUM || PUNCT}
@item @code{LOWER}  @tab @kbd{a-z}
@item @code{PRINT}  @tab @code{GRAPH ||} @kbd{space}
@item @code{PUNCT}  @tab @kbd{`~!@@#$%^&*()_-=+[@{]@}\|;:'",<.>/?}
@item @code{SPACE}  @tab @kbd{space tab \n \r \f \v}
@item @code{UPPER}  @tab @kbd{A-Z}
@item @code{XDIGIT} @tab @kbd{0-9A-Fa-f}
@end multitable

Note that, if the host character set is ASCII or a superset thereof,
all these macros will return false for all values of @code{char} outside
the range of 7-bit ASCII.  In particular, both ISPRINT and ISCNTRL return
false for characters with numeric values from 128 to 255.
@end itemize
@end deffn

@deffn  Extension ISIDNUM         (@var{c})
@deffnx Extension ISIDST          (@var{c})
@deffnx Extension IS_VSPACE       (@var{c})
@deffnx Extension IS_NVSPACE      (@var{c})
@deffnx Extension IS_SPACE_OR_NUL (@var{c})
@deffnx Extension IS_ISOBASIC     (@var{c})
These six macros are defined by @file{safe-ctype.h} and provide
additional character classes which are useful when doing lexical
analysis of C or similar languages.  They are true for the following
sets of characters:

@multitable {@code{SPACE_OR_NUL}} {yada yada yada yada yada yada yada yada}
@item @code{IDNUM}        @tab @kbd{A-Za-z0-9_}
@item @code{IDST}         @tab @kbd{A-Za-z_}
@item @code{VSPACE}       @tab @kbd{\r \n}
@item @code{NVSPACE}      @tab @kbd{space tab \f \v \0}
@item @code{SPACE_OR_NUL} @tab @code{VSPACE || NVSPACE}
@item @code{ISOBASIC}     @tab @code{VSPACE || NVSPACE || PRINT}
@end multitable
@end deffn

*/

#include "ansidecl.h"
#include <safe-ctype.h>
#include <stdio.h>  /* for EOF */

#if EOF != -1
 #error "<safe-ctype.h> requires EOF == -1"
#endif

/* Shorthand */
#define bl _sch_isblank
#define cn _sch_iscntrl
#define di _sch_isdigit
#define is _sch_isidst
#define lo _sch_islower
#define nv _sch_isnvsp
#define pn _sch_ispunct
#define pr _sch_isprint
#define sp _sch_isspace
#define up _sch_isupper
#define vs _sch_isvsp
#define xd _sch_isxdigit

/* Masks.  */
#define L  (const unsigned short) (lo|is   |pr)	/* lower case letter */
#define XL (const unsigned short) (lo|is|xd|pr)	/* lowercase hex digit */
#define U  (const unsigned short) (up|is   |pr)	/* upper case letter */
#define XU (const unsigned short) (up|is|xd|pr)	/* uppercase hex digit */
#define D  (const unsigned short) (di   |xd|pr)	/* decimal digit */
#define P  (const unsigned short) (pn      |pr)	/* punctuation */
#define _  (const unsigned short) (pn|is   |pr)	/* underscore */

#define C  (const unsigned short) (         cn)	/* control character */
#define Z  (const unsigned short) (nv      |cn)	/* NUL */
#define M  (const unsigned short) (nv|sp   |cn)	/* cursor movement: \f \v */
#define V  (const unsigned short) (vs|sp   |cn)	/* vertical space: \r \n */
#define T  (const unsigned short) (nv|sp|bl|cn)	/* tab */
#define S  (const unsigned short) (nv|sp|bl|pr)	/* space */

/* Are we ASCII? */
#if HOST_CHARSET == HOST_CHARSET_ASCII

const unsigned short _sch_istable[256] =
{
  Z,  C,  C,  C,   C,  C,  C,  C,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
  C,  T,  V,  M,   M,  V,  C,  C,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
  C,  C,  C,  C,   C,  C,  C,  C,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
  C,  C,  C,  C,   C,  C,  C,  C,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
  S,  P,  P,  P,   P,  P,  P,  P,   /* SP  !   "   #    $   %   &   '   */
  P,  P,  P,  P,   P,  P,  P,  P,   /* (   )   *   +    ,   -   .   /   */
  D,  D,  D,  D,   D,  D,  D,  D,   /* 0   1   2   3    4   5   6   7   */
  D,  D,  P,  P,   P,  P,  P,  P,   /* 8   9   :   ;    <   =   >   ?   */
  P, XU, XU, XU,  XU, XU, XU,  U,   /* @   A   B   C    D   E   F   G   */
  U,  U,  U,  U,   U,  U,  U,  U,   /* H   I   J   K    L   M   N   O   */
  U,  U,  U,  U,   U,  U,  U,  U,   /* P   Q   R   S    T   U   V   W   */
  U,  U,  U,  P,   P,  P,  P,  _,   /* X   Y   Z   [    \   ]   ^   _   */
  P, XL, XL, XL,  XL, XL, XL,  L,   /* `   a   b   c    d   e   f   g   */
  L,  L,  L,  L,   L,  L,  L,  L,   /* h   i   j   k    l   m   n   o   */
  L,  L,  L,  L,   L,  L,  L,  L,   /* p   q   r   s    t   u   v   w   */
  L,  L,  L,  P,   P,  P,  P,  C,   /* x   y   z   {    |   }   ~   DEL */

  /* high half of unsigned char is locale-specific, so all tests are
     false in "C" locale */
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
};

const unsigned char _sch_tolower[256] =
{
   0,  1,  2,  3,   4,  5,  6,  7,   8,  9, 10, 11,  12, 13, 14, 15,
  16, 17, 18, 19,  20, 21, 22, 23,  24, 25, 26, 27,  28, 29, 30, 31,
  32, 33, 34, 35,  36, 37, 38, 39,  40, 41, 42, 43,  44, 45, 46, 47,
  48, 49, 50, 51,  52, 53, 54, 55,  56, 57, 58, 59,  60, 61, 62, 63,
  64,

  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

  91, 92, 93, 94, 95, 96,

  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
  'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

 123,124,125,126,127,

 128,129,130,131, 132,133,134,135, 136,137,138,139, 140,141,142,143,
 144,145,146,147, 148,149,150,151, 152,153,154,155, 156,157,158,159,
 160,161,162,163, 164,165,166,167, 168,169,170,171, 172,173,174,175,
 176,177,178,179, 180,181,182,183, 184,185,186,187, 188,189,190,191,

 192,193,194,195, 196,197,198,199, 200,201,202,203, 204,205,206,207,
 208,209,210,211, 212,213,214,215, 216,217,218,219, 220,221,222,223,
 224,225,226,227, 228,229,230,231, 232,233,234,235, 236,237,238,239,
 240,241,242,243, 244,245,246,247, 248,249,250,251, 252,253,254,255,
};

const unsigned char _sch_toupper[256] =
{
   0,  1,  2,  3,   4,  5,  6,  7,   8,  9, 10, 11,  12, 13, 14, 15,
  16, 17, 18, 19,  20, 21, 22, 23,  24, 25, 26, 27,  28, 29, 30, 31,
  32, 33, 34, 35,  36, 37, 38, 39,  40, 41, 42, 43,  44, 45, 46, 47,
  48, 49, 50, 51,  52, 53, 54, 55,  56, 57, 58, 59,  60, 61, 62, 63,
  64,

  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

  91, 92, 93, 94, 95, 96,

  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
  'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

 123,124,125,126,127,

 128,129,130,131, 132,133,134,135, 136,137,138,139, 140,141,142,143,
 144,145,146,147, 148,149,150,151, 152,153,154,155, 156,157,158,159,
 160,161,162,163, 164,165,166,167, 168,169,170,171, 172,173,174,175,
 176,177,178,179, 180,181,182,183, 184,185,186,187, 188,189,190,191,

 192,193,194,195, 196,197,198,199, 200,201,202,203, 204,205,206,207,
 208,209,210,211, 212,213,214,215, 216,217,218,219, 220,221,222,223,
 224,225,226,227, 228,229,230,231, 232,233,234,235, 236,237,238,239,
 240,241,242,243, 244,245,246,247, 248,249,250,251, 252,253,254,255,
};

/* ASCII to EBCDIC(CP-1047) conversion table.  */
const unsigned char _sch_ascebc[256] =
{
 /*00  NL    SH    SX    EX    ET    NQ    AK    BL */
     0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,
 /*08  BS    HT    LF    VT    FF    CR    SO    SI */
     0x16, 0x05, 0x15, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
 /*10  DL    D1    D2    D3    D4    NK    SN    EB */
     0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,
 /*18  CN    EM    SB    EC    FS    GS    RS    US */
     0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
 /*20  SP     !     "     #     $     %     &     ' */
     0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
 /*28   (     )     *     +     ,     -    .      / */
     0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
 /*30   0     1     2     3     4     5     6     7 */
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
 /*38   8     9     :     ;     <     =     >     ? */
     0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
 /*40   @     A     B     C     D     E     F     G */
     0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
 /*48   H     I     J     K     L     M     N     O */
     0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
 /*50   P     Q     R     S     T     U     V     W */
     0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,
 /*58   X     Y     Z     [     \     ]     ^     _ */
     0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,
 /*60   `     a     b     c     d     e     f     g */
     0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
 /*68   h     i     j     k     l     m     n     o */
     0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
 /*70   p     q     r     s     t     u     v     w */
     0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
 /*78   x     y     z     {     |     }     ~    DL */
     0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x06, 0x17,
     0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x09, 0x0A, 0x1B,
     0x30, 0x31, 0x1A, 0x33, 0x34, 0x35, 0x36, 0x08,
     0x38, 0x39, 0x3A, 0x3B, 0x04, 0x14, 0x3E, 0xFF,
     0x41, 0xAA, 0x4A, 0xB1, 0x9F, 0xB2, 0x6A, 0xB5,
     0xBB, 0xB4, 0x9A, 0x8A, 0xB0, 0xCA, 0xAF, 0xBC,
     0x90, 0x8F, 0xEA, 0xFA, 0xBE, 0xA0, 0xB6, 0xB3,
     0x9D, 0xDA, 0x9B, 0x8B, 0xB7, 0xB8, 0xB9, 0xAB,
     0x64, 0x65, 0x62, 0x66, 0x63, 0x67, 0x9E, 0x68,
     0x74, 0x71, 0x72, 0x73, 0x78, 0x75, 0x76, 0x77,
     0xAC, 0x69, 0xED, 0xEE, 0xEB, 0xEF, 0xEC, 0xBF,
     0x80, 0xFD, 0xFE, 0xFB, 0xFC, 0xBA, 0xAE, 0x59,
     0x44, 0x45, 0x42, 0x46, 0x43, 0x47, 0x9C, 0x48,
     0x54, 0x51, 0x52, 0x53, 0x58, 0x55, 0x56, 0x57,
     0x8C, 0x49, 0xCD, 0xCE, 0xCB, 0xCF, 0xCC, 0xE1,
     0x70, 0xDD, 0xDE, 0xDB, 0xDC, 0x8D, 0x8E, 0xDF
};

/* EBCDIC(CP-1047) to ASCII conversion table.  */
const unsigned char _sch_ebcasc[256] =
{
 /*00  NU    SH    SX    EX    PF    HT    LC    DL */
     0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F,
 /*08              SM    VT    FF    CR    SO    SI */
     0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
 /*10  DE    D1    D2    TM    RS    NL    BS    IL */
     0x10, 0x11, 0x12, 0x13, 0x9D, 0x0A, 0x08, 0x87,
 /*18  CN    EM    CC    C1    FS    GS    RS    US */
     0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
 /*20  DS    SS    FS          BP    LF    EB    EC */
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x17, 0x1B,
 /*28              SM    C2    EQ    AK    BL       */
     0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
 /*30              SY          PN    RS    UC    ET */
     0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
 /*38                    C3    D4    NK          SB */
     0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
 /*40  SP                                           */
     0x20, 0xA0, 0xE2, 0xE4, 0xE0, 0xE1, 0xE3, 0xE5,
 /*48                     .     <     (     +     | */
     0xE7, 0xF1, 0xA2, 0x2E, 0x3C, 0x28, 0x2B, 0x7C,
 /*50   &                                           */
     0x26, 0xE9, 0xEA, 0xEB, 0xE8, 0xED, 0xEE, 0xEF,
 /*58               !     $     *     )     ;     ^ */
     0xEC, 0xDF, 0x21, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
 /*60   -     /                                     */
     0x2D, 0x2F, 0xC2, 0xC4, 0xC0, 0xC1, 0xC3, 0xC5,
 /*68                     ,     %     _     >     ? */
     0xC7, 0xD1, 0xA6, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
 /*70                                               */
     0xF8, 0xC9, 0xCA, 0xCB, 0xC8, 0xCD, 0xCE, 0xCF,
 /*78         `     :     #     @     '     =     " */
     0xCC, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
 /*80         a     b     c     d     e     f     g */
     0xD8, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
 /*88   h     i           {                         */
     0x68, 0x69, 0xAB, 0xBB, 0xF0, 0xFD, 0xFE, 0xB1,
 /*90         j     k     l     m     n     o     p */
     0xB0, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
 /*98   q     r           }                         */
     0x71, 0x72, 0xAA, 0xBA, 0xE6, 0xB8, 0xC6, 0xA4,
 /*A0         ~     s     t     u     v     w     x */
     0xB5, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
 /*A8   y     z                       [             */
     0x79, 0x7A, 0xA1, 0xBF, 0xD0, 0x5B, 0xDE, 0xAE,
 /*B0                                               */
     0xAC, 0xA3, 0xA5, 0xB7, 0xA9, 0xA7, 0xB6, 0xBC,
 /*B8                                 ]             */
     0xBD, 0xBE, 0xDD, 0xA8, 0xAF, 0x5D, 0xB4, 0xD7,
 /*C0   {     A     B     C     D     E     F     G */
     0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
 /*C8   H     I                                     */
     0x48, 0x49, 0xAD, 0xF4, 0xF6, 0xF2, 0xF3, 0xF5,
 /*D0   }     J     K     L     M     N     O     P */
     0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
 /*D8   Q     R                                     */
     0x51, 0x52, 0xB9, 0xFB, 0xFC, 0xF9, 0xFA, 0xFF,
 /*E0   \           S     T     U     V     W     X */
     0x5C, 0xF7, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
 /*E8   Y     Z                                     */
     0x59, 0x5A, 0xB2, 0xD4, 0xD6, 0xD2, 0xD3, 0xD5,
 /*F0   0     1     2     3     4     5     6     7 */
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
 /*F8   8     9                                     */
     0x38, 0x39, 0xB3, 0xDB, 0xDC, 0xD9, 0xDA, 0x9F
};

#else
# if HOST_CHARSET == HOST_CHARSET_EBCDIC
  #error "FIXME: write tables for EBCDIC"
# else
  #error "Unrecognized host character set"
# endif
#endif
