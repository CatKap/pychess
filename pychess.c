#include "defines.h"



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
        PyErr_SetString(PyExc_AttributeError, "last argument must be boolean");
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


PyObject *pychess_space_of_probs(PyObject *self, PyObject *args)
{
    PyObject *pos_list;
    long int figs_type;
    if(PyArg_ParseTuple(args, "Ol", &pos_list, &figs_type))         
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
        list *ret_list = brute_check(positions, 0, figs_type);        
        dprint("So, this for sure. Exit."); 
        PyObject *py_ret_list = PyList_New((Py_ssize_t)ret_list->lenth);

        int counter = 0;
        struct node *iterable = ret_list->first;
        while(iterable!= NULL)
        {
            PyObject *py_bite_probe = PyList_New(3);
            #ifdef _DEBUG
                if(iterable->data.position == 16) 
                    {
                        dprintd("counter", counter);
                        dprintd("list_len", ret_list->lenth);
                    }
            #endif
            PyList_SET_ITEM(py_bite_probe, 0, PyLong_FromLong((long)iterable->data.position));
            PyList_SET_ITEM(py_bite_probe, 1, PyLong_FromLong((long)iterable->data.posible_bite_position));
            PyList_SET_ITEM(py_bite_probe, 2, PyFloat_FromDouble((double)iterable->data.probabilyty));
            PyList_SET_ITEM(py_ret_list, counter, py_bite_probe);
            iterable = iterable->next;
            dprintd("CNT", counter);
            counter++;
        }

        return py_ret_list;
    }    
    else
    {
        PyErr_SetString(PyExc_AttributeError, "Fuction must take iterable object and number like argument.");
        return NULL;
    }
}

static PyMethodDef pychess_methods[] = {
    {"move", (PyCFunction)(void(*)(void))pychess_move, METH_VARARGS, NULL},
    {"is_position_bite", (PyCFunction)(void(*)(void))pychess_is_position_bite, METH_VARARGS, NULL},
    {"set_last_move", (PyCFunction)(void(*)(void))pychess_set_last_move, METH_VARARGS, NULL},
    {"space_of_probs", (PyCFunction)(void(*)(void))pychess_space_of_probs, METH_VARARGS, NULL},
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