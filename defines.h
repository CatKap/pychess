#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
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
#define IS_SIDE_WHITE 0x20 // mean lost side, in case of stalemate indicates whish king was stalemated, in case of draw indicates the last move side 

                           
#define trans_val(a, b) ((a) / 8 -  (b) / 8)
#define not_same_row(a, b) ((a) / 8 - (b) / 8)
#define not_same_colunm(a, b) ((a) % 8 - (b) % 8)
#define pos_busy(positions, pos) (positions[pos])
#define same_colunm(a, b) !not_same_colunm(a, b)
#define same_row(a, b) !not_same_row(a, b)
#define take_on_pass_white 0x30 // Mean thats white take on pass 
#define take_on_pass_black 0x50 // Mean thats black take on pass

extern char __GLOBAL_OLD_position;
extern char __GLOBAL_OLD_new_position;
extern char __GLOBAL_FLAG_is_last_move_data_correct;


// Structures of list, Ai functions

/* This struct are part of space of possible moves. 
Every stucture contain a position, where figure now stand;
possible position to bite (one at one node);
and probabilyty - for AI.
*/

typedef struct 
{
    char position; 
    char posible_bite_position;
    float probabilyty;
} bite_prob;

struct node
{
    char number;
    bite_prob data;
    struct node *next;
};

typedef struct 
{
    char lenth;
    struct node *first;
} list;

struct node *list_pop(list *lst, char index);
list *brute_check(char positions[64], char status, char colour);
struct node *list_pop(list *lst, char index);
struct node *fast_pop(list *list);

// Functions, using in pychess module
char c_move(signed char positions[64], unsigned char pos, unsigned char new_position, char *status);
int is_pos_biten(signed char positions[64], char pos, char is_white_bite, char* array);