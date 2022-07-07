#include "defines.h"

// Test fuction
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



// Function require to implement take on pass in engine. Set to gloabal varaibles move information. Use to set last from python code
PyObject *pychess_set_last_move(PyObject *self, PyObject *args)
{
    long int pos, new_pos, flag;
    if(!PyArg_ParseTuple(args, "lll", &pos, &new_pos, &flag))
    {
        PyErr_Print();
        PyErr_SetString(PyExc_AttributeError, "Invalid args for function call.");
        return Py_None;
    }
    __GLOBAL_OLD_new_position = (char)new_pos;
    __GLOBAL_OLD_position = (char)pos;
    __GLOBAL_FLAG_is_last_move_data_correct = (char)flag;
    return Py_None;
}

void *translate_positions(PyObject *py_positions, char positions[64])
{
    if (!PyList_Check(py_positions)) // py_positions must be a list of positions
    {
        PyErr_SetString(PyExc_AttributeError, "Positions is not a list!");
        return NULL;
    }

    if (!(PyList_Size(py_positions) == 64)) // Positions must have lenth equal to 64 
    {
        PyErr_SetString(PyExc_IndexError, "Invalid lenth of positions array, must be 64");
        return NULL;
    }

    for(char i = 0; i < 64; i++)
    {
        positions[i] = (signed char)PyLong_AsLong(PyList_GetItem(py_positions, (Py_ssize_t)i));
    }
	return (void*)1;
}

PyObject *pychess_move(PyObject *self, PyObject *args) // Function take a positions array, position (array index), new position and status
{
    long int position, new_position, l_status;
    signed char positions[64];
    PyObject *py_positions;
    if (!PyArg_ParseTuple(args, "Olll", &py_positions, &position, &new_position, &l_status))
    {
        PyErr_SetString(PyExc_AttributeError, "Invalid args for function call.");
        return NULL;
    }

	if(translate_positions(py_positions, positions) == NULL)
		return NULL;

    char status = (char)l_status;
    char move_flag = c_move(positions, position, new_position, &status);

    if(!(status & PARTY_END))
    {
        if (move_flag) // Return a True of False
            status = status | MOVE_APPLYED;
        else
            status &= ~MOVE_APPLYED;
    }
    dprintd("STATUS", status);
    PyObject *py_ret_positions = PyList_New((Py_ssize_t) 64);
    for (int i = 0; i < 64; i++) // Creatae a returned positions list
    {
       PyList_SET_ITEM(py_ret_positions, (Py_ssize_t)i, PyLong_FromLong((long) positions[i]));  
    }    
    
    // If take on pass happend, three elements of array required
    char size;
    if (move_flag & take_on_pass_black || move_flag & take_on_pass_white)
    {
        size = 3;
    }
    else
        size = 2;
    
    PyObject *ret_list = PyList_New(size);
    // Next two if-s set the position of taked pawn when take on pass happend
    // About take on pass alert the third element of array
    if ((move_flag & ~True) == take_on_pass_black)
    {
        dprint("---------------->WHITE PAWN POSITION SET");
        PyObject *white_pawn_pos = PyLong_FromLong((long)new_position + 8); 
        PyList_SET_ITEM(ret_list, 2, white_pawn_pos);      
    }
    if ((move_flag & ~True) == take_on_pass_white) 
    {
        dprint("---------------->BLACK PAWN POSITION SET");
        PyObject *black_pawn_pos = PyLong_FromLong((long)new_position - 8);
        PyList_SET_ITEM(ret_list, 2, black_pawn_pos);
    }
    PyList_SET_ITEM(ret_list, 0, py_ret_positions);
    PyList_SET_ITEM(ret_list, 1, PyLong_FromLong((long)status));
    return ret_list;
}

//Sees if position bite, return array, where first number says how many fiuges bites this position, and it positions in last part of array
// Does not pay attention to king, check, mate, etc. 
PyObject* pychess_is_position_bite(PyObject *self, PyObject *args)
{
    signed char positions[64];
    long suspect_position;
    char is_white_bite;
    PyObject  *py_is_white_bite;
    PyObject *py_positions;
    
    if(!PyArg_ParseTuple(args, "OlO", &py_positions, &suspect_position, &py_is_white_bite))
    {
        PyErr_SetString(PyExc_AttributeError, "Bad agruments for function call");
        return NULL;
    }
    
    if (PyBool_Check(py_is_white_bite))
    {
        if (py_is_white_bite == Py_True)
        {
            is_white_bite = True;
        }
        else
        {
            is_white_bite = False;
        }
    }
    else
    {
        PyErr_SetString(PyExc_AttributeError, "Last argument must be boolean");
        return NULL;
    }
    if(!(PyList_Size(py_positions) == 64))
    {
        PyErr_SetString(PyExc_IndexError, "Invalid lenth of positions array, must be 64");
        return NULL;
    }
    for (int i = 0; i < 64; i++)
    {
        positions[i] = (signed char)PyLong_AsLong(PyList_GetItem(py_positions, (Py_ssize_t)i));
    }
    char array[16]; // More space 
    PyObject *py_ret_list = PyList_New(1);
    long int ret = (long)is_pos_biten(positions, (signed char)suspect_position, is_white_bite, array);
    
    PyList_SET_ITEM(py_ret_list, 0, PyLong_FromLong((long)array[0])); // count on first pos

    for(int i = 1; i < ret + 1; i++)
    {
        PyList_Append(py_ret_list, PyLong_FromLong((long)array[i]));
    }
    return py_ret_list;
}


// Returns space of possible moves in array [ [position, new_position], ..., ]
PyObject *pychess_space_of_probs(PyObject *self, PyObject *args)
{
    PyObject *pos_list;
    long int figs_type, status;
    if(PyArg_ParseTuple(args, "Oll", &pos_list, &status, &figs_type))         
    {
        if(!PyList_Check(pos_list)) // pos_list must be list
        {
            PyErr_SetString(PyExc_AttributeError, "list of positions must be list");
            return NULL;            
        }

        if(!(PyList_Size(pos_list) == 64))
        {
            PyErr_SetString(PyExc_IndexError, "List of positions must contain 64 elements.");
            return Py_None;
        }

        char positions[64]; 
        for(char i = 0; i < 64; i++)
        {
            positions[i] = (char)PyLong_AsDouble(PyList_GetItem(pos_list, (ssize_t)i));
        }

        dprintd("fig_type", figs_type);
        dprint("Not error.");
		size_t lenth = 1;
    	list *ret_list = new_brute_check(positions, (char)status, figs_type);
        dprint("So, this for sure. Exit."); 
        PyObject *py_ret_list = PyList_New((Py_ssize_t)ret_list->lenth);

#ifdef _DEBUG
		list *sec_list = brute_check(positions, (char)status, figs_type);
        struct node *itr = sec_list->first;
		printf("TEST: [");
		while(itr != NULL)
		{
			printf("[%d, %d], ", itr->data.position, itr->data.posible_bite_position);
			itr = itr->next;	
		}
		printf("]\n");
#endif

        int counter = 0;
        struct node *iterable = ret_list->first;
        while(iterable != NULL)
        {
            PyObject *py_bite_probe = PyList_New(2);
            PyList_SET_ITEM(py_bite_probe, 0, PyLong_FromLong((long)iterable->data.position));
            PyList_SET_ITEM(py_bite_probe, 1, PyLong_FromLong((long)iterable->data.posible_bite_position));
            PyList_SET_ITEM(py_ret_list, counter, py_bite_probe);
            iterable = iterable->next;
            dprintd("CNT", counter);
            counter++;
        }
        return py_ret_list;
    }    
    else
    {
        PyErr_SetString(PyExc_AttributeError, "Fuction must take list of positions (64 elements), status and number (which define side of checking) like argument.");
        return NULL;
    }
}

// AI fuctions
// Return a addres of agent in python int type
PyObject *pychess_init_agent(PyObject *self, PyObject *args)
{
	long int depth, side; 
	PyObject *py_positions;
	if(!PyArg_ParseTuple(args, "llO", &depth, &side,  &py_positions))
	{
		PyErr_SetString(PyExc_AttributeError, "Bad agrguments");
		return NULL;
	}
	struct agent *ret_agent_addres = malloc(sizeof(struct agent));
	if (translate_positions(py_positions, (char*)ret_agent_addres->positions) == NULL)
			return NULL;

	// Append depth and side of agent
	ret_agent_addres->depth = (unsigned char)depth;
	ret_agent_addres->status = (char)side;
	return PyLong_FromLong((long long)ret_agent_addres);
}

PyObject *pychess_delete_agent(PyObject *self, PyObject *args)
{
	long long addres;
	if(!PyArg_ParseTuple(args, "l", &addres))
	{
		PyErr_SetString(PyExc_AttributeError, "Bad agrguments");
		return NULL;
	}
	// Test code for agent here
		

	struct agent *d_agent = (void *)addres;
	dprint("===You delete the agent===");
	dprintd("WITH DEPTH", d_agent->depth);
	dprintd("WITH STATUS", d_agent->status);
	print_desk((char*)d_agent->positions, 0);
	free(d_agent);
	return Py_None;
}

// Python module required stack
static PyMethodDef pychess_methods[] = {
    {"move", (PyCFunction)(void(*)(void))pychess_move, METH_VARARGS, NULL},
    {"is_position_bite", (PyCFunction)(void(*)(void))pychess_is_position_bite, METH_VARARGS, NULL},
    {"set_last_move", (PyCFunction)(void(*)(void))pychess_set_last_move, METH_VARARGS, NULL},
    {"space_of_probs", (PyCFunction)(void(*)(void))pychess_space_of_probs, METH_VARARGS, NULL},
    {"init_agent", (PyCFunction)(void(*)(void))pychess_init_agent, METH_VARARGS, NULL},
    {"delete_agent", (PyCFunction)(void(*)(void))pychess_delete_agent, METH_VARARGS, NULL},
    { NULL, NULL, 0, NULL}
};

static PyModuleDef pychess_module = {
    PyModuleDef_HEAD_INIT,
    "pychess",
    "Module for faster execution of some chess algorytms.",
    0,
    pychess_methods
};

PyMODINIT_FUNC PyInit_pychess(void) // Function called while going command "import pychess"
{
    return PyModule_Create(&pychess_module);
}
