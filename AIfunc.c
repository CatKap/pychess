#include "defines.h"
// Some functions for AI require

list *list_create()
{
   list *new = malloc(sizeof(list));
   dprint("List creation pass!");
   new->lenth = 0;
   new->first = NULL; 
   return new;
}
	
void list_add(list *lst, bite_prob dt)
{
    #ifdef _DEBUG
        dprint("+++++++++++++++++++++++ADD DEBUG 16");
        dprintd("pos", dt.position);
        dprintd("posb", dt.posible_bite_position);
    #endif
    struct node *new = malloc(sizeof(struct node));
    new->number = lst->lenth;
    new->data.position = dt.position;
    new->data.posible_bite_position = dt.posible_bite_position;
    new->next = lst->first; 

    lst->first = new;
    lst->lenth += 1; dprintd("LENTH", lst->lenth);
}

struct node *list_get(list *lst, char index)
{
    if(lst->lenth == 0 || lst->lenth < index)
        return NULL;

    struct node *search = lst->first;
    while (search->next != NULL)
    {
        if(search->number == index)
            return search;
        search = search->next;
    }
	return NULL; // Just for sure
}

struct node *list_pop(list *lst, char index)
{
    if(abs(index) >= lst->lenth) 
        return NULL;

    if(index < 0)
        index += lst->lenth; 

    struct node *previos = lst->first; 
    struct node *search;  
    struct node *ret;
    
    if(lst->lenth > 1)
        search = previos->next;
    else
    {
        if(index == 0 && lst->lenth == 1)
        {
            return previos;
            lst->lenth--;
        }
        else
            return NULL;
    }
    
    while (search->next != NULL)
    {
        if (search->number > index)
        {
            search->number--;
        }

        if(search->number == index) 
        {
            previos->next = search->next;
            ret = search;
        }
        search = search->next;
        previos = search;
    }
    lst->lenth--;
    return ret;
}

struct node *fast_pop(list *list)
{
    if(list->lenth > 0)
    {
        struct node *ret = list->first;
        list->first = ret->next;

        struct node *index_nd = list->first;
        do
        {
            if(index_nd->number > ret->number && index_nd != NULL) 
                index_nd->number--;
            index_nd=index_nd->next;
        }
        while(index_nd->next != NULL);
        list->lenth--;
        return ret; 
    }
    return NULL; // ERRNO SEE
}

// This function returns two-dimensional pointer-like array  of probable desks if sec_dim parametr is not zero, list of probable space of moves   
// *sec_dim is a mutable variable thats answer about lenth of second dimention (first lenth is constant - 64) desks_array array
// signed char* positions must be positions[64]
// side === is_white_bite
list* new_brute_check(signed char positions[64], char status, char side, previos_move last_move)
{
	list *ret_list = list_create();
	bite_prob prob_move;
	unsigned char bite_array[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	
	unsigned char bite_counter = 0;
	for(char i = 0; i < 64; i++) // For all positions
	{
		bite_counter = is_pos_biten(positions, i, side, bite_array);
		for(char x = 1; x <= bite_counter; x++)
		{
			if(try_move(positions, bite_array[x], i, status, last_move))
			{
				prob_move.position = bite_array[x];
				prob_move.posible_bite_position = i;
				list_add(ret_list, prob_move);
			}
		}
	}
	return ret_list;
}

char** build_desks_array(list *prob_list, signed char original_positinons[64])
{
	char** desks_array = (char**)malloc(sizeof(char*) * prob_list->lenth);
	dprint(">>> Build of problist continue");
	dprint("	Start building a array");
	struct node *cnt_node = prob_list->first;
	desks_array =(char**)malloc(sizeof(char) * prob_list->lenth);	
	dprint("	First array init pass");
	for(char i = 0; i < prob_list->lenth; i++)	
	{
		dprintd("	Sec arr num", i);
		*(desks_array + i)= (char*)malloc(64);
		dprint("	Init sec array with i num");
		for(char j = 0; j < 64; j++)
		{
			(desks_array[i])[j] = original_positinons[j];
			dprintd("	Pass this J", j);
		}
		dprint("	Pass J cycle");
		dprintd("	cnt_node->data.posible_bite_position", cnt_node->data.posible_bite_position);
		// Applying a position
		(desks_array[i])[cnt_node->data.posible_bite_position] = (*(desks_array + i))[cnt_node->data.position];
		(desks_array[i])[cnt_node->data.position] = 0;
		cnt_node = cnt_node->next; // Because cycle use lenth of list like upper limit, cnt_node runs all the list nodes
		dprintd("Pass i", i);
	}
	dprint("Exit ->build_desks_array<-");
	return desks_array;
}

// Soft check using when old probable desk exsist, so just change it
void soft_check(struct agent *agnt, char *sec_dim, char **desks_array)
{

}


list* brute_check(char positions[64], char status, char colour, previos_move last_move)
{
    list *ret_list = list_create();
    bite_prob next;
    for(char i = 0; i < 64; i++)
    {
        if(positions[i] > 0 && colour || positions < 0 && !colour)
        {
            for(char pos = 0; pos < 64; pos++)
            {
                if(try_move(positions, i, pos, status, last_move))
                {
                    next.position = i;
                    next.posible_bite_position = pos;
                    list_add(ret_list, next);
                }
            }
        }
    }
    return ret_list;
}

float fig_cost(char figure)
{
    switch(figure)
    {
        case 1: case -1:
            return 1;

        case 2: case -2: case 3: case -3:
            return 3;
        
        case 4: case -4:
            return 5;
            
        case 5: case -5:
            return 9;

        case 6: case -6:
            return -1;
        default:
            return 0;
    }
}


void positions_copy(char positions[64], char new[64])
{
	for (char i = 0; i < 64; i++)
	{
		new[i] = positions[i];
	}
}


// If you wanna call it, see chesstest (contain call of this function in it's main func)
int ai_main(void)
{
    char positions[64] = {
    4, 2, 3, 5, 6, 3, 2, 4, 
    1, 1, 1, 1, 1, 1, 1, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
   -1,-1,-1,-1,-1,-1,-1,-1, 
   -4,-2,-3,-5,-6,-3,-2,-4};
	previos_move lst_m = {0, 0, -100};
    list *space = brute_check(positions, 0, True, lst_m); // First desk init 
    struct node *nd = space->first;
    while(nd->next != NULL)
    {
        printf("%d -> %d\n", nd->data.position, nd->data.posible_bite_position);
        nd = nd->next;
    }
    printf("Total is: %d", space->lenth);
}

