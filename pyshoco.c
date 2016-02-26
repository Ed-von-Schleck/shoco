#include <Python.h>
#include <string.h>
#include "shoco.h"

#if PY_MAJOR_VERSION >= 3
#define IS_PY3K
#endif

static PyObject *pyshoco_compress(PyObject *self, PyObject *args);
static PyObject *pyshoco_decompress(PyObject *self, PyObject *args);

static PyMethodDef ShocoMethods[] = {
    {"compress",  pyshoco_compress, METH_VARARGS,
     "Compress a string."},
    {"decompress",  pyshoco_decompress, METH_VARARGS,
     "Decompress a string."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

#ifdef IS_PY3K

static struct PyModuleDef shocomodule = {
   PyModuleDef_HEAD_INIT,
   "pyshoco",   /* name of module */
   NULL, /* module documentation, may be NULL */
   -1,       /* size of per-interpreter state of the module,
                or -1 if the module keeps state in global variables. */
   ShocoMethods
};

PyMODINIT_FUNC PyInit_pyshoco(void)
{
    return PyModule_Create(&shocomodule);
}

#else
    
PyMODINIT_FUNC initpyshoco(void)
{
    (void) Py_InitModule("pyshoco", ShocoMethods);
}

#endif

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