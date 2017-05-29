#include <Python.h>
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

const char *bytesFormat = "y#";

#else
    
PyMODINIT_FUNC initpyshoco(void)
{
    (void) Py_InitModule("pyshoco", ShocoMethods);
}

const char *bytesFormat = "s#";

#endif

static PyObject *pyshoco_compress(PyObject *self, PyObject *args) {
    const char *in;
    size_t insize;

    if (!PyArg_ParseTuple(args, "s#", &in, &insize))
        return NULL;
    
    size_t outsize = insize*2+1;
    char out[outsize];
    outsize = shoco_compress(in, insize, out, outsize);
    return Py_BuildValue(bytesFormat, out, outsize);
}

static PyObject *pyshoco_decompress(PyObject *self, PyObject *args) {
    const char *in;
    size_t insize;

    if (!PyArg_ParseTuple(args, bytesFormat, &in, &insize))
        return NULL;
    
    size_t outsize = insize*2+1;
    char out[outsize];
    outsize = shoco_decompress(in, insize, out, outsize);
    return Py_BuildValue("s#", out, outsize);
}
