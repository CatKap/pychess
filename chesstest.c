#include <stdio.h>
#include "defines.h"

//long figures[] = {"♔", "♕", "♖", "♗", "♘", "♙", "♚", "♛"};




void print_desk(char positions[64], unsigned char tested)
{
    for(char y = 7; y >= 0; y--)
    {
        for(char x = 0; x < 8; x++)
        {
            if (8 * y + x == tested && tested != 0)
            {
                printf(" n ");
                continue;
            }
            if (positions[8 * y + x] >= 0)                
                printf(" %d ", positions[8 * y + x]);
            else
                printf("-%d ", positions[8 * y + x] * -1);
        }
        printf("\n");
    }
}


char write_position(char positions[64],  char position, char value)
{ 
   unsigned char pos = (unsigned char)(position);  
   if (pos < 64)
        positions[pos] = value;
    else
        return False;
    return True;
}


int figure_is_blocked_test(char figure)
{
    char positions[64];
    for(char b = 0; b < 64; b++)
    {
        positions[b] = 0;
    }
                        
    for(char i = 0; i < 64; i++)
    {
        printf("%d\n", i);
        positions[i] = figure;
        char k_point;
        for(char _new = 0; _new < 64; _new++)
        {
            if(is_one_step_transfer_applyed(i, _new, figure))
                write_position(positions, _new, 1);
        }
        printf("Answer %d\n", figure_is_blocked(positions, i, i)); 
        print_desk(positions, i);
        printf("NOT HERE");
        printf("\n");
        for(char b = 0; b < 64; b++)
        {
            positions[b] = 0;
        }
    }
	return 0;
}

void arr_mt_test(char array[5])
{
	array[0] = 5;
}

int main(void)
{
	char positions [64] = 
	{4, 2, 3, 5, 6, 3, 2, 4, 
     1, 1, 1, 0, 1, 1, 1, 1, 
	 0, 0, 0, 0, 0, 0, 0, 0, 
	 0, 0, 0, 1, 0, 0, 0, 0, 
	 0, 0, 0, 0, 0, 0, 0, 0, 
	 0, 0, 0, 0, 0, 0, 0, 0, 
	-1,-1,-1,-1,-1,-1,-1,-1, 
	-4,-2,-3,-5,-6,-3,-2,-4};

	size_t lnt = 1;	
	previos_move p_move = {-1, -1, 0};
	list *ret_list = brute_check(positions, 0, 1, p_move); 
	struct node *itr = ret_list->first; 	
	printf("TEST: [");
	while(itr != NULL)
	{
		printf("[%d, %d], ", itr->data.position, itr->data.posible_bite_position);
		itr = itr->next;	
	}
	printf("]\n");
	return 0;
}





