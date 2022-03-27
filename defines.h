#include <stdlib.h>
#include <stdio.h>
//#define _DEBUG
#ifdef _DEBUG
    #define dprint(str_val)  printf("%s\n", str_val) 
    #define dprintd(name, int_val) printf("%s: %d\n", name, int_val)
#else
    #define dprint(null) //asm("NOP")
    #define dprintd(null, null1) //asm("NOP")
#endif

// Use just in char type! 
#define True 1
#define False 0
#define not(x) ((~x) & 1)
//Status define
#define WHITE_CASTLING_RIGHT_DENIED 0x01
#define WHITE_CASTLING_LEFT_DENIED 0x02
#define BLACK_CASTLING_RIGHT_DENIED 0x04
#define BLACK_CASTLING_LEFT_DENIED 0x08
#define WHITE_ON_CHECK 0x10
#define BLACK_ON_CHECK 0x20
#define MOVE_APPLYED 0x40
// If party end the fist bit is high: 1 _ _ _ _ _ _ _ (num is lower than zero)
#define PARTY_END 0x80
#define IS_MATE 0x40
#define IS_SIDE_WHITE 0x20 // mean lost side
// 
#define trans_val(a, b) ((a) / 8 -  (b) / 8)
#define not_same_row(a, b) ((a) / 8 - (b) / 8)
#define not_same_colunm(a, b) ((a) % 8 - (b) % 8)
#define pos_busy(positions, pos) (positions[pos])
#define same_colunm(a, b) !not_same_colunm(a, b)
#define same_row(a, b) !not_same_row(a, b)

// Functions, using in pychess module
char c_move(signed char positions[64], unsigned char pos, unsigned char new_position, char *status);

int is_pos_biten(signed char positions[64], char pos, char is_white_bite, char* array);