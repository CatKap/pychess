#include "defines.h"
#include <Python.h>

PyObject *pychess_move(PyObject *self, PyObject *args) // Function take a positions array, position (array index), new position and status
{
    long int position, new_position, l_status;
    signed char positions[64];
    PyObject *py_positions;
    if (!PyArg_ParseTuple(args, "Olll", &py_positions, &position, &new_position, &l_status))
    {
        PyErr_Print();
        PyErr_SetString(PyExc_AttributeError, "Invalid args for function call.");
        return Py_None;
    }

    if (!PyList_Check(py_positions)) // py_positions must be a list of positions
    {
        PyErr_Print();
        PyErr_SetString(PyExc_AttributeError, "Positions is not a list!");
        return Py_None;
    }

    if (!(PyList_Size(py_positions) == 64)) // Positions must have lenth equal to 64 
    {
        PyErr_Print();
        PyErr_SetString(PyExc_IndexError, "Invalid volume of positions, must be 64");
        return Py_None;
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
    PyObject *ret_list = PyList_New(2);
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
        dprint("One");
        return Py_None;
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
        dprint("Two");
        return Py_None;
    }
    if(!(PyList_Size(py_positions) == 64))
    {
        dprint("Three");
        return Py_None;
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
            PyErr_BadArgument();
            return Py_None;            
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
        PyErr_SetString(PyExc_AttributeError, "Fuction take iterable object and number like argument.");
        return Py_None;
    }
}

static PyMethodDef pychess_methods[] = {
    {"move", (PyCFunction)(void(*)(void))pychess_move, METH_VARARGS, NULL},
    {"is_position_bite", (PyCFunction)(void(*)(void))pychess_is_position_bite, METH_VARARGS, NULL},
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