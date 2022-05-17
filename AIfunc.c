#include "defines.h"
// Some functions for AI require

list *list_create()
{
   list *new = malloc(sizeof(list));
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
    new->data.probabilyty = dt.probabilyty;
    new->next = lst->first; 

    lst->first = new;
    lst->lenth += 1;
    dprintd("LENTH", lst->lenth);
}

char list_append_list(list *lst, list *appended, char number)
{
    struct node *search = lst->first;
    while (search->next != NULL)
    {
        if(search->number == number)
        {
            search->nextlist = appended;
            return True;
        }
        search = search->next;
    }
    return False;
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

list* brute_check(char positions[64], char status, char colour)
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
        nd = nd->next;
        nd->data.probabilyty = prob;
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
    list *space = brute_check(positions, 0, True); // First desk init 
    struct node *nd = space->first;
    while(nd->next != NULL)
    {
        printf("%d -> %d, Prob: %f\n", nd->data.position, nd->data.posible_bite_position, nd->data.probabilyty);
        nd = nd->next;
    }
    printf("Total is: %d", space->lenth);
}



