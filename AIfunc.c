#define _DEBUG
#include "defines.h"
// Some functions for AI require

/* This struct are part of space of possible moves. 
Every stucture contain a position, where figure now stand;
possible position to bite (one at one node);
and probabilyty - for AI.
*/

// Structures of list, Ai functions
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

list* list_create()
{
   list *new = malloc(sizeof(list));
   new->lenth = 0;
   new->first = NULL; 
   return new;
}

void list_add(list *lst, bite_prob dt)
{
    struct node *new = malloc(sizeof(struct node));
    new->number = lst->lenth;
    new->data.position = dt.position;
    new->data.posible_bite_position = dt.posible_bite_position;
    new->data.probabilyty = dt.probabilyty;
    new->next = lst->first; 

    lst->first = new;
    lst->lenth += 1;
    dprintd("LENTH", lst->lenth);
}

struct node* list_get(list *lst, char index)
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
}

struct node* list_pop(list *lst, char index)
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

list* init_desk(char positions[64], char status, char colour)
{
    list *ret_list = list_create();
    bite_prob next;
    for(char i = 0; i < 64; i++)
    {
        if(positions[i] > 0 && colour || positions < 0 && !colour)
        {
            for(char pos = 0; pos < 64; pos++)
            {
                if(c_move(positions, i, pos, &status))
                {
                    next.position = i;
                    next.posible_bite_position = pos;
                    next.probabilyty = 0; 
                    list_add(ret_list, next);
                }
            }
        }
    }
    struct node *nd = ret_list->first;
    float prob = 1 / (float)(ret_list->lenth);
    while(nd->next != NULL)
    {
        nd->data.probabilyty = prob;
        nd = nd->next;
    }
    return ret_list;
}


int main(void)
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
    list *space = init_desk(positions, 0, True);
    struct node *nd = space->first;
    while(nd->next != NULL)
    {
        printf("%d -> %d, Prob: %f\n", nd->data.position, nd->data.posible_bite_position, nd->data.probabilyty);
        nd = nd->next;
    }
    printf("Total is: %d", space->lenth);
}



