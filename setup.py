from setuptools import setup
from setuptools import Extension

setup(
	name='adcf',
	version='1',
	ext_modules=[
		Extension(
			'adcf',
			['funcs.c'],
			extra_objects = ["alu.o"],
			library_dirs = ["/usr/lib/x86_64-linux-gnu/"],
			libraries = ["cuda", "cudadevrt", "cudart"]
		)
	]
)
