#include "defines.h"
char king_points[8] = {-9, -8, -7, -1, 1, 7, 8, 9};
char knight_points[8] = {-17, -15, -10, -6, 6, 10, 15, 17};
char queen_steps[8] = {-9, -7, 7, 9, -8, -1, 1, 8};
char* bishop_steps = queen_steps;
char* tower_steps = queen_steps + 4;
char __GLOBAL_OLD_position = -1;
char __GLOBAL_OLD_new_position = -1;
char __GLOBAL_FLAG_is_last_move_data_correct = False;

char is_pos_exsist(unsigned char pos)
{
    if (pos >= 64)
        return False;
    return True;
}

char is_figures_have_same_colour(char fig_1, char fig_2)
{
    if ((fig_1 > 0 && fig_2 > 0) || (fig_1 < 0 && fig_2 < 0))
        return True;
    return False;
}

void add_element(char* array, char element, char num)
{
    if (array != NULL)
        array[num] = element;
}

char is_transfer_applyed(unsigned char pos1, unsigned char pos2, char figure)
{
    if(!is_pos_exsist(pos1))
        return False;
    
    if(!is_pos_exsist(pos2))
        return False;

    if(pos1 == pos2)
        return False;

    signed char delta;
    switch(figure)
    {
        case 1:
            if (trans_val(pos1, pos2) == 1 && same_colunm(pos1, pos2))         
                return True;
            break;

        case 2: case -2:
            delta = pos2 - pos1;
            switch(trans_val(pos2, pos1))
            {
                case 1:
                   switch(delta) { case 10: case 6: return True;}
                   break;
                case 2:
                   switch(delta) { case 17: case 15: return True;}
                   break;
                case -1:
                    switch(delta) { case -10: case -6: return True;}
                   break;
                case -2:
                    switch(delta) { case -17: case -15: return True;}
                    break;
            }
            break;
        case 3: case -3:
            delta = pos2 - pos1;
            if (delta % 7 == 0)
            {
                if (delta / 7 == trans_val(pos2, pos1))
                    return True;               
            }
            if (delta % 9 == 0)   
            {
                if (delta / 9 == trans_val(pos2, pos1))
                    return True;
            }
            break;
        case 4: case -4:
            if (same_colunm(pos1, pos2) || same_row(pos1, pos2))
                return True;               
            break;
               
        case 5: case -5:
            if (is_transfer_applyed(pos1, pos2, 3) || is_transfer_applyed(pos1, pos2, 4))
                return True;
            break;
        
        case 6: case -6:
            signed char trans_val = trans_val(pos2, pos1);
            delta = pos2 - pos1; 
            switch (delta)
            {
                case -9: case -8: case -7:
                    if(trans_val == -1)
                        return True;
                    break;

                case 1: case -1:
                    if(trans_val == 0) 
                        return True;
                    break;

                case 9: case 8: case 7:
                    if(trans_val == 1) 
                        return True;
                    break;
            }
            break;
    } 
    return False;
}

char is_one_step_transfer_applyed(unsigned char pos1, unsigned char pos2, char figure)
{
    if (is_transfer_applyed(pos1, pos2, figure))
    {
        signed char delta = pos2 - pos1;

        switch (figure)
        {
        case 5: case -5:
            if(same_colunm(pos1, pos2) && abs(delta) == 8) 
                return True;
            else
            {
                if(same_row(pos1, pos2) && abs(delta) == 1)
                    return True;
                else // Means the diagonal move
                {
                    if ((abs(delta) == 7 || abs(delta) == 9) && abs(trans_val(pos1, pos2)) == 1)
                        return True;
                }
            }
            break;

        case 4: case -4:
            if(same_row(pos1, pos2) && abs(delta) == 1)
                return True;
            else
            {
                if(same_colunm(pos1, pos2) && abs(delta) == 8)
                    return True;
            }
            break;

        case 3: case -3:
            if(abs(trans_val(pos1, pos2)) == 1 && not_same_colunm(pos1, pos2) && not_same_row(pos1, pos2))            
                return True;
            break;

        case 1: case -1:
            return True; // Pawn always make one step move

        default:
            break;
        }
        return False;
    }
}

int is_pos_biten(signed char positions[64], char pos, char is_white_bite, char* array) //FIXME!
{                                 
    if (!is_pos_exsist(pos)) 
    {
        dprintd("POSITION NOT EXSIST!", pos);
        return False;
    }
    // Vertical check
    unsigned char suspected_pos;
    signed char suspected_figure;
    char count = 0;
    char points[4] = {-1, 8, 1, -8}; // clockwise of straight lines
    for (char point = 1; point < 8; point++)
    {
        for (char i = 0; i < 4; i++)
        {
            suspected_figure = positions[pos + point * points[i]];
            suspected_pos =  pos + point * points[i];
            // points[i] = 0 mean stop the checking
            if (abs(trans_val(pos, suspected_pos)) > point) // Requiere becasue function must be uncutble 
                points[i] = 0;
            if(!is_pos_exsist(suspected_pos))
                points[i] = 0;
            if (points[i] != 0 && suspected_figure != 0)
            {
                // Next two if-s is the topological
                if ((points[i] == 1 || points[i] == -1) && not_same_row(suspected_pos, pos))
                {
                    points[i] = 0;
                    continue;
                }
                if ((points[i] == 8 || points[i] == -8) && not_same_colunm(suspected_pos, pos))
                {
                    points[i] = 0;
                    continue;
                }
                if ((suspected_figure > 0 && is_white_bite) || (suspected_figure < 0 && !is_white_bite)) 
                {
                    // On position is enemy figure.
                    switch(abs(suspected_figure))
                    {
                        case 5: case 4:
                            count++;                        
                            add_element(array, suspected_pos, count);
                            break;
                    
                        case 6:
                            if (point == 1 && abs(trans_val(suspected_pos, pos)) <= 1)
                            {    
                                count++;
                                add_element(array, suspected_pos, count);
                            }
                            break;
                    }
                }
                points[i] = 0;
            }
            if (suspected_figure != 0)
                points[i] = 0;
        }
    }
    // Diagonal check
    points[0] = -9; points[1] = 7; points[2] = 9; points[3] = -7;

    for (char point = 1; point < 8; point++)
    {
        for (char i = 0; i < 4; i++)
        {
            suspected_pos = pos + points[i] * point;
            suspected_figure = positions[pos + point * points[i]];
            if (abs(trans_val(suspected_pos, pos)) != point) // Not diagonal checking
                points[i] = 0;
            if(!is_pos_exsist(suspected_pos))
                points[i] = 0;
            if (points[i] != 0 && suspected_figure != 0)
            {
                if ((suspected_figure > 0 && is_white_bite) || (suspected_figure < 0 && !is_white_bite))
                {       
                    switch(abs(suspected_figure))
                    {
                        case 5: case 3:                            
                            count++;
                            add_element(array, suspected_pos, count);
                            break;
                        case 6:                  
                            if (point == 1 && abs(suspected_pos / 8 - pos / 8) <= 1)
                            {
                                count++;
                                add_element(array, suspected_pos, count);
                            }
                            break;
                    }
                    points[i] = 0;
                }
            }
            if (suspected_figure != 0)
                points[i] = 0;
        }
    }
    // Pawns check
    if (is_white_bite && ((pos - 7) / 8 - pos / 8) == -1 && positions[pos - 7] == 1 && is_pos_exsist(pos - 7))
        {
           count++;
           add_element(array, pos - 7, count); 
        }
    if (is_white_bite && ((pos - 9) / 8 - pos / 8) == -1 && positions[pos - 9] == 1 && is_pos_exsist(pos - 9))
        {
            count++;
            add_element(array, pos - 9, count);
        }
    if (!is_white_bite && ((pos + 7) / 8 - pos / 8) == 1 && positions[pos + 7] == -1 && is_pos_exsist(pos + 7))
        {
            count++;
            add_element(array, pos + 7, count);
        }
    if (!is_white_bite && ((pos + 9) / 8 - pos / 8) == 1 && positions[pos + 9] == -1 && is_pos_exsist(pos + 9))
        {
            count++;
            add_element(array, pos + 9, count);
        }

    // Knights check
    for (char i = 0; i < 8; i++)
    {
        if (!is_pos_exsist(pos + knight_points[i]))
            continue;
        if (is_white_bite && positions[pos + knight_points[i]] == 2 || !is_white_bite && positions[pos + knight_points[i]] == -2)
        {
            switch(trans_val(pos + knight_points[i], pos))
            {
                case 1:
                    if (knight_points[i] == 6 || knight_points[i] == 10)
                    {
                        count++;
                        add_element(array, pos + knight_points[i], count);
                    }
                    break;
                case 2:
                    if (knight_points[i] == 15 || knight_points[i] == 17)
                    {
                        // Pos right, white knight on it
                        count++;
                        add_element(array, pos + knight_points[i], count);
                    }
                    break;
                case -1:
                    if (knight_points[i] == -6 || knight_points[i] == -10)
                    {
                        count++;
                        add_element(array, pos + knight_points[i], count);
                    }
                    break;
                case -2:
                    if (knight_points[i] == -15 || knight_points[i] == -17)
                    {
                        count++;
                        add_element(array, pos + knight_points[i], count);
                    }
                    break;
            }
        }
    }
    add_element(array, count, 0);
     
    return count;
}

/* make_sure function moves figure and check will it move provide king check, and if it does, return False. In another case (move not provide king check) returns true
*/
char make_sure(signed char positions[64], unsigned char pos, unsigned char new_pos, unsigned char king_pos)
{   
    char flag = True;
    char colour; 
    if (positions[pos] < 0)
       colour = False; 
    else 
        colour = True;    
    char _old = positions[new_pos];
    positions[new_pos] = positions[pos];
    positions[pos] = 0;
    if (is_pos_biten(positions, king_pos, not(colour), NULL)) 
    {
        flag = False;
    }
    positions[pos] = positions[new_pos];
    positions[new_pos] = _old;
    return flag;
}

char king_is_blocked(signed char positions[64], char king_position, char king_type)
{
    dprint("KING CHECK START"); 
    for(char i = 0; i < 8; i++)
    {
        unsigned char suspected_pos = king_position + king_points[i];
        dprintd("KP", king_points[i]);

        // -6 + 12 * king_type because if black, king_type must be zero, in another case -6 + 12 = 6, white king
        if(!is_transfer_applyed(king_position, suspected_pos, -6 + 12 * king_type))
            continue;
        
        dprintd("SUS_POSB", is_pos_biten(positions, suspected_pos, not(king_type), NULL));
        dprintd("SUS_P", suspected_pos);
        dprintd("NOT", not(king_type));
        if (!is_pos_biten(positions, suspected_pos, not(king_type), NULL))
        {
            dprint("GET THIS");
            if (!pos_busy(positions, suspected_pos))
            {
                dprint("First not pass!");
                dprintd("count value i", king_position + king_points[i]); //FIXME !!!
                return False;
            }
            // Position is not bite
            if (king_type && positions[suspected_pos] < 0 || !king_type &&positions[suspected_pos] > 0) 
            {
                dprint("Sec fail.");
                dprintd("   positions[susp_pos]", positions[suspected_pos]);
                return False;
            }
        }
    }
    dprint("TRUE RET");
    return True;
}

char figure_is_blocked(signed char positions[64], unsigned char position, unsigned char king_position) 
{
    dprint("FIG IS BLOCKED CALL"); 
    dprintd("AT POSITION", position);
    dprintd("KING ON POSITION", king_position);
    char lenth = 8;
    char *array = queen_steps;
    char colour;
    char possible_moves[] = {0};
    char king_type;
    char figure = positions[position];
    if (positions[position] < 0)
       colour = -1; 
    else 
        colour = 1;    
    dprintd("REAL POS ANSWER:", positions[position]);
    switch(abs(positions[position]))
    { 
        case 1: 
            for (char i = 7; i <= 9; i += 2) 
            {
               if (trans_val(position + i, position) == 1 * colour)              
                {
                    if (pos_busy(positions, position + i * colour) && !is_figures_have_same_colour(positions[position], positions[position + i * colour]))
                    {
                        if(!make_sure(positions, position, position + i * colour, king_position)) 
                            return False;   
                    }    
                }
            }
            dprint("PAWN CHECK");
            if (trans_val(position + 8 * colour, position) == 1 * colour && !pos_busy(positions, position + 8 * colour)) 
                if(make_sure(positions, position, position + 8 * colour, king_position)) 
                { 
                    dprint("PAWN WORK FINE");
                    return False;  
                }
            dprint("Return true"); 
            return True;
            break;
        case 2:
            for (char i = 0; i < 8; i++)
            {
                if (is_transfer_applyed(position, position + knight_points[i], 2))                
                {
                    if (!pos_busy(positions, position + knight_points[i]) || !is_figures_have_same_colour(positions[position], positions[position + knight_points[i]]))
                    { 
                        if(make_sure(positions, position, position + knight_points[i], king_position))
                            return False;
                    }
                }

            }
            return True;
            break;

        case 3:
            lenth = 4; 
            break;

        case 4:
            lenth = 4;
            array += 4; // Take pointer at vertical part of queen array
            break; 

        case 6: case 0:
            char king_type;

            if(colour == 1)
                king_type = True;
            else
                king_type = False;

            return king_is_blocked(positions, king_position, king_type);
           break; 

        case 5: case -5:
            array = king_points;
            break; // Avoid default exit, all parametrs for queen already applyed
        default:
            dprint("NOT A FIGURE AT POSITION. EXIT");
            return False;
            break;
    }
        unsigned char s_pos;
        for(char i = 0; i < lenth; i++)
        {
            s_pos = position + array[i];

            if(!is_pos_exsist(s_pos))
                continue;

            if(is_one_step_transfer_applyed(position, s_pos, positions[position]))
            {
                if (!pos_busy(positions, position) || pos_busy(positions, position) && !is_figures_have_same_colour(positions[s_pos], positions[position]))
                {             
                    if(make_sure(positions, position, s_pos, king_position))
                        return False;
                }
            }
        }
    return True;
}

// If mate returns 1 (True), if stalemate, returns -1 if no mate returns 0 (False) 
char mate_stalemate_check(signed char positions[64], unsigned char king_position, char king_type) // king_type True if king white
{
    dprint("CALL MATE AND STALEMATE CHECK");
    dprintd("KING TYPE", king_type);
    positions[king_position] = 0;

    if (king_is_blocked(positions, king_position, king_type))
    {
        char bite_array[16];
        dprint("KING CAN'T MOVE");
        
        if (is_pos_biten(positions, king_position, not(king_type), bite_array))
        {
            
             dprintd("SECMV", bite_array[0]);
            if (bite_array[0] > 1) // King can't move and some figure bite it. 
            { 
                dprint("MATE!");
                goto ret_true; // Mate mean
            }
            else
            {    
                dprintd("Just one bite the king", bite_array[1]);  
                
                /* Two stages of checking:
                    1) If we can kill figure, that's mated the king - not mate
                    2) If we can stand our figure between king and mated */
                
                char ret[16];
                if(is_pos_biten(positions, bite_array[1], king_type, ret)) // If position of figure, what mate are biten by another figures
                {
                    for(char i = 1; i <= ret[0]; i++) // For all of figures, that bite the figure, what mated the king
                    {
                        if(make_sure(positions, ret[i], bite_array[1], king_position)) // If bite of mate king figure not provide another check 
                            goto ret_false; // If just of one can bite, exit. Not mate
                    }
                }
                signed char trans_val = trans_val(bite_array[1], king_position); 
                switch (positions[bite_array[1]]) // If this one figure what bite is knight or pawn, exit, because it can't be blocked 
                {
                case 1: case -1: goto ret_true; break;
                case 2: case -2: goto ret_true; break; 
                } 
                dprint("//////FIRST OUT");
                char direction;
                char s_pos; // suspected_position
                char step;
                if(trans_val != 0)
                {
                    step = abs(trans_val);
                    direction = (bite_array[1] - king_position) / abs(trans_val);
                }
                else
                {
                    direction = (bite_array[1] - king_position) / abs((bite_array[1] - king_position));
                    step = abs(bite_array[1] - king_position);
                }
                dprintd("//// DIRECTION", direction);
                dprintd("//// STEP", step);
                for (char i = 1; i < step; i++)
                {
                    s_pos = direction * i + king_position;
                    if(is_pos_biten(positions, s_pos, king_type, ret))
                    {
                        
                        for(char x = 1; x <= ret[0]; x++)
                        {
                            // dprintd("//// RET[x]", ret[x]);
                            // dprintd("//// SUSP_POS", s_pos);
                            if(make_sure(positions, ret[x], s_pos, king_position))
                                goto ret_false;
                        }
                    }
                }
                // All checks out
                goto ret_true;

            }
        }
        else
        {
            // Stalemate checking    

            for (char pos = 0; pos < 64; pos++)
            {
                if (king_type && positions[pos] > 0 || !king_type && positions[pos] < 0)
                {
                    if(!figure_is_blocked(positions, pos, king_position))
                    {
                        dprint("/EXIT/");
                        goto ret_false;
                    }
                }
            }
            return -1; // This is stalemate
        }
    }
    ret_false:
        dprint("RETURN FALSE FROM MATE_STALEMATE_CHEK");
        dprintd("   King is", king_type);
        if (king_type)
            positions[king_position] = 6;
        else
            positions[king_position] = -6;
        return False;

    ret_true:
        if (king_type)
            positions[king_position] = 6;
        else
            positions[king_position] = -6;
        return True;
    
}


char kings_check(signed char positions[64], signed char fig_type, char *status) // Make some neeadble checks for kings. Modifate status
{
    // fig type is a type of figure, thats moving last time
    // king's find
    char flag = True;
    char white_king_position, black_king_position;
    for (char i = 0; i < 64; i++)
    {
        if (positions[i] == 6)
        {
            white_king_position = i;
            break;
        }
    }
    for (char i = 0; i < 64; i++)
    {
        if (positions[i] == -6)
        {
            black_king_position = i;
            break;
        }
    }
    // Black king checking
    if (is_pos_biten(positions, black_king_position, True, NULL))
    { 
        if (*status & BLACK_ON_CHECK && fig_type < 0)
        {    
            
            flag = False;
        }
        else
        {
            *status = *status | BLACK_ON_CHECK;
            dprint("Status applyed");
            if(mate_stalemate_check(positions, black_king_position, False) == 1)
                {
                    dprint("BLACK MUST DIE! ///////");
                    *status = 0 | PARTY_END | IS_MATE; // Side is black  
                }
        }
        if (fig_type < 0)
            flag = False;
    }       
    else
    {    
        if (mate_stalemate_check(positions, black_king_position, False) == -1)
            *status = 0 | PARTY_END;
        else
            *status &= ~BLACK_ON_CHECK;
    }

    // White king checking
    if (is_pos_biten(positions, white_king_position, False, NULL))
    {
        if (*status & WHITE_ON_CHECK && fig_type > 0 )
            flag = False;
        else
        {
            *status = *status | WHITE_ON_CHECK;
            if(mate_stalemate_check(positions, white_king_position, True) == 1)
            {
                dprint("WHITE MUST DIE");
                *status = 0 | PARTY_END | IS_MATE | IS_SIDE_WHITE;
            }
        }
        if (fig_type > 0)
            flag = False;
    }
    else
    {
        if (mate_stalemate_check(positions, white_king_position, True) == -1)
            *status = 0 | PARTY_END | IS_SIDE_WHITE;
        else
            *status = *status & ~WHITE_ON_CHECK;
    }
    positions[white_king_position] = 6;
    positions[black_king_position] = -6;
    return flag;
}

char apply_move(signed char positions[64], unsigned char pos, unsigned char new_position, char *status)
{
    dprint("///////CALL APPLY MOVE");
    signed char fig_type = positions[pos];
    signed char old = positions[new_position];
    positions[pos] = 0;
    positions[new_position] = fig_type;
    char flag = kings_check(positions, fig_type, status);
    if (!flag)
    {
        positions[pos] = fig_type;
        positions[new_position] = old;
    }
    return flag;
}

char take_on_pass_check(char positions[64])
    {
        dprint("----------------ENTER IN take_on_pass_check---------------");
        if(!__GLOBAL_FLAG_is_last_move_data_correct)
        {
            dprint("EXIT FROM take_on_pass");
            return False;
        }
        dprint("CHECK PASS");
        dprint("-----IF DATA-----");
        dprintd("OLD_pos_fig", abs(positions[__GLOBAL_OLD_new_position]));
        dprintd("TR_VAL", trans_val(__GLOBAL_OLD_new_position, __GLOBAL_OLD_position));
        if(abs(positions[__GLOBAL_OLD_new_position]) == 1 && abs(trans_val(__GLOBAL_OLD_new_position, __GLOBAL_OLD_position)) == 2) 
        {
            return True;
        }
        return False;
    }


// That function return True (1) if move applyed and modificate status, if that's necessary
char c_move(signed char positions[64], unsigned char pos, unsigned char new_position, char *status)
{
    if (*status < 0) // Party end
        return False;
    
    if (positions[pos] < 0 && positions[new_position] < 0 || positions[pos] > 0 && positions[new_position] > 0) // If on new_position is a same colour figure, return False
        return False;
    
    if (abs(positions[new_position]) == 6) // See to not bite a king
        return False;

    
    signed char trans_val = new_position / 8 - pos / 8;
    signed char fig_type = positions[pos];
    char _old;
    switch (fig_type)
    {
        case 1: // White pawn 
            if (!pos_busy(positions, new_position))
            {
                if (new_position == pos + 8 && trans_val == 1)
                {
                    return apply_move(positions, pos, new_position, status);
                }
                if (new_position == pos + 16 && !pos_busy(positions, pos + 8) &&  trans_val == 2 && pos > 7 && pos < 16)
                {
                    return apply_move(positions, pos, new_position, status);
                }

                if (new_position == pos + 7 || new_position == pos + 9) 
                {
                    dprint("-----------------TAKE ON PASS CHECK ENTER-----------------");
                    dprintd("ONP", __GLOBAL_OLD_new_position);
                    dprintd("OP", __GLOBAL_OLD_position);
                    dprintd("FLAG", __GLOBAL_FLAG_is_last_move_data_correct);
                    dprint("------IF DATA-----");
                    dprintd("pos[n_pos - 8]", positions[new_position - 8]);
                    dprintd("take_on_pass_check", take_on_pass_check(positions));
                    if (positions[new_position - 8] == -1 && take_on_pass_check(positions))
                    {
                        dprint("ENTER");
                        positions[new_position - 8] = 0;
                        if (apply_move(positions, pos, new_position, status))
                        {
                            dprint("------------------> ToP white returned");
                            return True | take_on_pass_white;
                        }
                    }
                }
            }
            else // That's will be bite 
            {
                if (trans_val == 1) // If trans_val ok and bited figure is black
                {
                    if (positions[new_position] < 0)
                    {
                        if (new_position == pos + 7 || new_position == pos + 9)
                        {
                            return apply_move(positions, pos, new_position, status);
                        }   
                    }
                }
            }
            break;
        case -1: // Black pawn
            if (!pos_busy(positions, new_position))
          {
                if (new_position == pos - 8 && trans_val == -1)
                {
                    return apply_move(positions, pos, new_position, status);
                }
                if (new_position == pos - 16 && !pos_busy(positions, pos - 8) &&  trans_val == -2 && pos > 47 && pos < 56)
                {
                    return apply_move(positions, pos, new_position, status);
                }

                if (new_position == pos - 7 || new_position == pos - 9) 
                {
                    if (positions[new_position + 8] == 1 && take_on_pass_check(positions))
                    {
                        dprint("ENTER BLACK");
                        positions[new_position + 8] = 0;
                        if (apply_move(positions, pos, new_position, status))
                        {
                            dprint("------------------> ToP black returned");
                            return True | take_on_pass_black;
                        }
                    }
                }
            }
            else // That's will be bite 
            {                
                if (trans_val == -1 && positions[new_position] > 0)
                {
                    //  s, new_position);
                    if (new_position == pos - 7 || new_position == pos - 9)
                    {
                        return apply_move(positions, pos, new_position, status);
                    }
                }   
            }            
            break;
        case 2: case -2: // Knights
            char flag = False;
            switch (trans_val)
            {
                case 1:
                    if (new_position == pos + 6 || new_position == pos + 10)
                        flag = True;
                break;
                case 2:
                    if (new_position == pos + 17 || new_position == pos + 15)
                        flag = True;
                break;
                case -1:
                    if (new_position == pos - 6 || new_position == pos - 10)
                        flag = True;
                break;
                case -2:
                    if (new_position == pos - 17 || new_position == pos - 15)
                        flag = True;
                break;
            }
            if (flag)
                return apply_move(positions, pos, new_position, status);
            break;
        case -3: case 3: // Bishops
            unsigned char checking_pos;
            if ((new_position - pos) % 7 == 0) // The new_position on 7 diegonal
            {
                if (trans_val == (new_position - pos) / 7)
                {
                    if (trans_val > 0)
                    {
                        for (int i = 1; i < trans_val; i++)
                        {
                            checking_pos = 7 * i + pos;
                            if(pos_busy(positions, checking_pos))
                                return False;
                        }
                        return apply_move(positions, pos, new_position, status);
                    }
                    else
                    {
                        for (int i = 1; i < -trans_val; i++)
                        {
                            checking_pos = -7 * i + pos;
                            if(pos_busy(positions, checking_pos))
                                return False;
                        }
                        return apply_move(positions, pos, new_position, status);
                    }
                }
            }
            if ((new_position - pos) % 9 == 0) // The new_position on 9 diegonal
            {
                if (trans_val == (new_position - pos) / 9) 
                {
                    if (trans_val > 0)
                    {       
                        for (int i = 1; i < trans_val; i++)
                        {
                            checking_pos = 9 * i + pos;
                            if(pos_busy(positions, checking_pos))
                            {
                                return False;
                            }
                        }
                        return apply_move(positions, pos, new_position, status);
                    }
                    else
                    {
                        for (int i = 1; i < -trans_val; i++)
                        {
                            checking_pos = -9 * i + pos;
                            if(pos_busy(positions, checking_pos))
                                return False;
                        }
                        return apply_move(positions, pos, new_position, status);
                    }
                }
            }    
            break;

        case 4:  case -4: // Castles
            if (trans_val == 0)
            {
                if ((new_position - pos) > 0)
                {
                    for (char i = (pos + 1); i < new_position; i++)
                        {
                            if (pos_busy(positions, i))
                                return False;
                        }
                }
                else
                {
                    for (char i = (pos - 1); i > new_position; i--)
                        {
                            if (pos_busy(positions, i))
                                return False;
                        }
                }
            }
            else
            {
                if((new_position - pos) % 8 == 0) // If positions stays on a colunm
                {
                    if ((new_position - pos) > 0)
                    {
                        for (char i = 1; i < (new_position - pos) / 8; i++)
                        {
                            if (pos_busy(positions, pos + i * 8))
                                return False;
                                
                        }
                    }
                    else
                    {
                        for (char i = -1; i > (new_position - pos) / 8; i--)
                        {
                            if (pos_busy(positions, pos + i * 8))
                                return False;
                        }                        
                    }
                }
                else
                    return False;

            }
            // Move applyed. In another case False returned
            char castling_status;
            switch (pos)
            {
                case 0: // Left bottom corner (White left)
                    castling_status = WHITE_CASTLING_LEFT_DENIED;
                break;
                case 7: // Right bottom corner (White right)
                    castling_status = WHITE_CASTLING_RIGHT_DENIED;
                break;
                case 56: // Left top corner (Black left)
                    castling_status = BLACK_CASTLING_LEFT_DENIED;
                break;
                case 63: // Right top corner (Black right)
                    castling_status = BLACK_CASTLING_RIGHT_DENIED;
                break;
                default: // If tower is not on corner positon
                    castling_status = 0x00;
                break;
            }
            if (apply_move(positions, pos, new_position, status))
            {
                *status = *status | castling_status; 
                return True;
            }
            else
                return False;
            break;
        case 5: // The Queen is a bishop and castle, so move called like a bishop and like a castle
            _old = positions[new_position];
            positions[pos] = 3;
            if(c_move(positions, pos, new_position, status))
            {
                positions[pos] = 5;
                positions[new_position] = _old;
                return apply_move(positions, pos, new_position, status);
            }
            else
            {
                positions[pos] = 4;
                if(c_move(positions, pos, new_position, status))
                {
                    positions[pos] = 5;
                    positions[new_position] = _old;
                    return apply_move(positions, pos, new_position, status);
                }
                else
                {
                    positions[pos] = 5;
                    positions[new_position] = _old;
                }                
            }
            return False; 
            break;
        case -5:
            _old = positions[new_position];
            positions[pos] = -3;
            if(c_move(positions, pos, new_position, status))
            {
                positions[pos] = -5;
                positions[new_position] = _old;
                return apply_move(positions, pos, new_position, status);
            }
            else
            {
                positions[pos] = -4;
                if(c_move(positions, pos, new_position, status))
                {
                    positions[pos] = -5;
                    positions[new_position] = _old;
                    return apply_move(positions, pos, new_position, status);
                }
                else
                {
                    positions[pos] = -5;
                    positions[new_position] = _old;
                }                
            }
            return False; 
            break;
        
        case 6:
            if (new_position == 2 && !(*status & WHITE_CASTLING_LEFT_DENIED)) // Try catling white left we are know, the castle and king not moving yet
            {
                if(!(pos_busy(positions, 3) || pos_busy(positions, 2) || pos_busy(positions, 1) || *status & WHITE_ON_CHECK))
                {
                    if (!(is_pos_biten(positions, 3, False, NULL) || is_pos_biten(positions, 2, False, NULL) || is_pos_biten(positions, 1, False, NULL) || is_pos_biten(positions, 0, False, NULL)))
                    {
                        if (apply_move(positions, pos, new_position, status))
                        {
                           *status = *status | WHITE_CASTLING_LEFT_DENIED;
                           *status = *status | WHITE_CASTLING_RIGHT_DENIED;
                           positions[3] = 4;
                           positions[0] = 0;
                           return True;
                        }
                    }   
                }
            }
            if (new_position == 6 && !(*status & WHITE_CASTLING_RIGHT_DENIED))
            {
                if(!(pos_busy(positions, 5) || pos_busy(positions, 6) || *status & WHITE_ON_CHECK))
                {
                    if (!(is_pos_biten(positions, 5, False, NULL) || is_pos_biten(positions, 6, False, NULL) || is_pos_biten(positions, 7, False, NULL)))
                    {
        
                        if (apply_move(positions, pos, new_position, status))
                        {
                           *status = *status | WHITE_CASTLING_LEFT_DENIED;
                           *status = *status | WHITE_CASTLING_RIGHT_DENIED;
                           positions[5] = 4;
                           positions[7] = 0;
                           return True;
                        }
                    }   
                }
            }
        case -6:
            if (new_position == 58 && !(*status & BLACK_CASTLING_LEFT_DENIED)) // Try castling white left we are know, the castle and king not moving yet
            {
                dprint("CATLR");
                if(!(pos_busy(positions, 59) || pos_busy(positions, 58) || pos_busy(positions, 57) || *status & BLACK_ON_CHECK))
                {
                    if (!(is_pos_biten(positions, 59, True, NULL) || is_pos_biten(positions, 58, True, NULL) || is_pos_biten(positions, 57, True, NULL) || is_pos_biten(positions, 56, True, NULL)))
                    {
                        if (apply_move(positions, pos, new_position, status))
                        {
                           *status = *status | BLACK_CASTLING_LEFT_DENIED;
                           *status = *status | BLACK_CASTLING_RIGHT_DENIED;
                           positions[59] = -4;
                           positions[56] = 0;
                           return True;
                        }
                    } 
                }
            }
            if (new_position == 62 && !(*status & BLACK_CASTLING_RIGHT_DENIED))
            {
                if(!(pos_busy(positions, 61) || pos_busy(positions, 62) || *status & BLACK_ON_CHECK))
                {
                    dprint("ONE");
                    if (!(is_pos_biten(positions, 61, True, NULL) || is_pos_biten(positions, 62, True, NULL) || is_pos_biten(positions, 63, True, NULL)))
                    {
                        dprint("TWO");
                        if (apply_move(positions, pos, new_position, status))
                        {
                           *status = *status | BLACK_CASTLING_LEFT_DENIED;
                           *status = *status | BLACK_CASTLING_RIGHT_DENIED;
                           positions[63] = 0;
                           positions[61] = -4;
                           return True;
                        }
                    }   
                }
            }
            
            if (trans_val == 0)
            {
                if ((new_position - pos) == 1 || (new_position - pos) == -1)
                {    
                    if(apply_move(positions, pos, new_position, status))
                    {
                        *status = *status | WHITE_CASTLING_LEFT_DENIED;
                        *status = *status | WHITE_CASTLING_RIGHT_DENIED;
                        return True;
                    }
                }
            }

            if (trans_val == 1  || trans_val == -1)
                if ((new_position - pos < 10 && new_position - pos > 6) || ((new_position - pos) > -10 && (new_position - pos) < -6))
                {
                    if(apply_move(positions, pos, new_position, status))
                    {
                        *status = *status | WHITE_CASTLING_LEFT_DENIED;
                        *status = *status | WHITE_CASTLING_RIGHT_DENIED;
                        return True;
                    }
                }
            break;
    }
    return False;
}