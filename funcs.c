/* This particular implementation of add_cuda uses OS files to store state. I do
  think that it would be a better idea to implement a way of storing state
  internally as well... (I will need to figure out how to create a custom object
  or somethng...)
*/

/* This implementation is *far* less buggy than the first implementation */

#define PY_SSIZE_T_CLEAN

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <cuda.h>
#include <cuda_runtime.h>
#include <Python.h>

void _Z11load_kernelPiS_S_(int *ar1, int *ar2, int *out);

static PyObject
*add_cuda(PyObject *self)
{
  int arr1[32];
  int arr2[32];
  int arro[32];
  int *dar1;
  int *dar2;
  int *dout;
  int fd1;
  int fd2;
  int fdo;

  if (cudaMalloc((void *)&dar1, sizeof(int) * 32) != cudaSuccess)
    goto memerrb;
  if (cudaMalloc((void *)&dar2, sizeof(int) * 32) != cudaSuccess)
    goto memerrb;
  if (cudaMalloc((void *)&dout, sizeof(int) * 32) != cudaSuccess)
    goto memerrb;

  goto memerre;
memerrb:
  PyErr_SetString(PyExc_MemoryError, "cudaMalloc() failed.");
  return NULL;
memerre:

  fd1 = open("./file1", O_RDONLY);
  fd2 = open("./file2", O_RDONLY);
  fdo = open("./fileo", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

  if ((fd1 == -1) || (fd2 == -1))
  {
    PyErr_SetString(PyExc_FileNotFoundError,
      "Could not open './file1', or './file2'.");
    return NULL;
  }

  if (fdo == -1)
  {
    PyErr_SetString(PyExc_Exception,
      "Unknown error occured opening './fileo'.");
    return NULL;
  }

  read(fd1, arr1, sizeof(int) * 32);
  read(fd2, arr2, sizeof(int) * 32);

  cudaMemcpy(dar1, arr1, sizeof(int) * 32, cudaMemcpyHostToDevice);
  cudaMemcpy(dar2, arr2, sizeof(int) * 32, cudaMemcpyHostToDevice);

  _Z11load_kernelPiS_S_(dar1, dar2, dout);

  cudaMemcpy(arro, dout, sizeof(int) * 32, cudaMemcpyDeviceToHost);

  cudaFree(dar1);
  cudaFree(dar2);
  cudaFree(dout);

  write(fdo, arro, sizeof(int) * 32);

  close(fd1);
  close(fd2);
  close(fdo);

  return PyUnicode_FromString("OK");
}

static PyObject
*write_file(PyObject *self, PyObject *args)
{
  char str1[] = "arr1";
  char str2[] = "arr2";
  char *cur_edit_str;
  int fd;
  int fid;

  fd = 0;

  if (!PyArg_ParseTuple(args, "i", &fid))
  {
    PyErr_SetString(PyExc_TypeError, "Usage: write_file(int fileno).");
    return NULL;
  }

  if (fid > 2 || fid < 1)
  {
    PyErr_SetString(PyExc_Exception,
      "Invalid fileno specified. It is required to be either 1, or 2.");
    return NULL;
  }

  if (fid == 1)
  {
    fd = open("./file1", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    cur_edit_str = str1;
  }

  if (fid == 2)
  {
    fd = open("./file2", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    cur_edit_str = str2;
  }

  if (fd == -1)
  {
    PyErr_SetString(PyExc_Exception, "File open error occured.");
    return NULL;
  }

  if (fd == 0)
  {
    PyErr_SetString(PyExc_Exception, "A very strange error has occured.");
    return NULL;
  }

  for (int i = 0; i < 32; i++)
  {
    int cw;

    printf("%s[%02i] = ", cur_edit_str, i);
    scanf("%i", &cw);
    write(fd, &cw, sizeof(int));
  }

  close(fd);

  return PyUnicode_FromString("OK");
}

static PyObject
*print_output(PyObject *self)
{
  int fd;

  fd = open("./fileo", O_RDONLY);

  if (fd == -1)
  {
    PyErr_SetString(PyExc_FileNotFoundError,
      "The file './fileo' likely does not exist.");
    return NULL;
  }

  for (int i = 0; i < 32; i++)
  {
    int integer;

    read(fd, &integer, sizeof(int));

    printf("%i ", integer);
  }

  printf("\n");

  close(fd);

  return PyUnicode_FromString("OK");
}

static struct PyMethodDef methods[] =\
{
  {"add_cuda", (PyCFunction)add_cuda, METH_NOARGS},
  {"write_file", (PyCFunction)write_file, METH_VARARGS},
  {"print_output", (PyCFunction)print_output, METH_NOARGS},
  {NULL, NULL}
};

static struct PyModuleDef module =\
{
  PyModuleDef_HEAD_INIT,
  "adcf",
  NULL,
  -1,
  methods
};

PyMODINIT_FUNC
PyInit_adcf(void)
{
  return PyModule_Create(&module);
}
