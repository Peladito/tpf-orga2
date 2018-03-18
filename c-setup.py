from distutils.core import setup, Extension
setup(name='asa_c', version='1.0', ext_modules=[Extension('asac', ['asa_c.c'])])
