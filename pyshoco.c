#include <Python.h>
#include <string.h>
#include "shoco.h"

static PyObject *pyshoco_compress(PyObject *self, PyObject *args) {
    const char *in;

    if (!PyArg_ParseTuple(args, "s", &in))
        return NULL;
    
	const size_t bufsize = strlen(in)*2+1;
	char out[bufsize];
	memset(out, 0, bufsize);
	shoco_compress(in, strlen(in), out, bufsize);
    return Py_BuildValue("s", out);
}

static PyObject *pyshoco_decompress(PyObject *self, PyObject *args) {
    const char *in;

    if (!PyArg_ParseTuple(args, "s", &in))
        return NULL;
    
	const size_t bufsize = strlen(in)*2+1;
	char out[bufsize];
	memset(out, 0, bufsize);
	shoco_decompress(in, strlen(in), out, bufsize);
    return Py_BuildValue("s", out);
}

static PyMethodDef ShocoMethods[] = {
    {"compress",  pyshoco_compress, METH_VARARGS,
     "Compress a string."},
    {"decompress",  pyshoco_decompress, METH_VARARGS,
     "Decompress a string."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC initpyshoco(void)
{
    (void) Py_InitModule("pyshoco", ShocoMethods);
}