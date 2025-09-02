# python-add-cuda-file

My second implementation of python-add-cuda. This one actually works properly (or so it seems) :p

# Requirements

* Linux (Can be converted to work on Windows)
* CUDA toolkit
* CPython (version 3)
* Pip package manager

# Build + install

First, the alu.cu file must be built into an object library file:

`nvcc -lib -o alu.o ./alu.cu`

Then, use the Pip installer:

`pip install .`

If the install command does not work, please check the "library_dirs" variable in the setup.py file. You may have to specify where your
CUDA libraries reside.

# Usage
```
> import adcf
> adcf.write_file(1)
  ...
> adcf.write_file(2)
  ...
> adcf.add_cuda()
> print_output()
234 54 34 ...
```

# Bugs + things to note

* This implementation uses hardcoded files to store state, which may be slightly user unfriendly.
* The 2 arrays that are to be added, are stored in 2 files, with the ID of 1 and 2.
* The output is stored in a separate file.
* Error handling is not the best (file opens do not make a distinction between errors, a write to file could fail without ack).
* If the addcuda function gives you a GPU memory leak error without probable cause, delete adc, and repeat *"Build + install"*.
