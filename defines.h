#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#define _DEBUG
#ifdef _DEBUG
    #define dprint(str_val)  printf("%s\n", str_val) 
    #define dprintd(name, int_val) printf("%s: %d\n", name, (int)int_val)
#else
    #define dprint(null) //asm("NOP")
    #define dprintd(null, null1) //asm("NOP")
#endif

//#define WHITE_PAWN
//#define WHITE_KNIGHT
//#define WHITE_BISHOP
//#define WHITE_

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
								
// Agent status defines 
// Agent side swithable
#define AGENT_WHITE_SIDE 0x01
#define AGENT_BLACK_SIDE 0x00
#define AGENT_JUST_INITILIZE 0x02 // Requre because first space of probable moves find out by brute force (64 * 64 moves try)

extern char knight_points[];
extern char* bishop_steps; // This is array, does not fool yourself

// Global variables contain informainiton about previos move - needable for take on pass
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
} bite_prob;

struct node
{
    char number;
    bite_prob data;
    struct node *next;
};

typedef struct 
{
    size_t lenth;
    struct node *first;
} list;

struct agent // Contains all data for ML algorytms proccecing    
{
	unsigned int depth;   // Recursion depth
	signed char positions[64];   // Array for positions
	char status;          // Contains information about agent in flag-like style
	char party_status;    // This status is status of patry
	list *space_of_moves; // Contains a list of probable moves 
};

typedef struct // Require for take on pass check
{
	unsigned char old_position;
	unsigned char new_position;
	signed char new_position_figure;
	
} previos_move;

struct node *list_pop(list *lst, char index);
list *brute_check(char positions[64], char status, char colour);
struct node *list_pop(list *lst, char index);
struct node *fast_pop(list *list);

// Functions, using in pychess module
char c_move(signed char positions[64], unsigned char pos, unsigned char new_position, char *status);
char try_move(signed char positions[64], unsigned char pos, unsigned char new_position, char status);
int is_pos_biten(signed char positions[64], char pos, char is_white_bite, char* array);
char is_one_step_transfer_applyed(unsigned char pos1, unsigned char pos2, char figure);
char figure_is_blocked(signed char positions[64], unsigned char position, unsigned char king_position);
list* new_brute_check(signed char positions[64], char status, char side);
char** build_desks_array(list *prob_list, signed char original_positinons[64]);
list *list_create();






