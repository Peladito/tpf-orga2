from distutils.core import setup, Extension
setup(name='asa-mimd.c', version='1.0', ext_modules=[Extension('asaMIMD', ['asa-mimd.c'],extra_objects=["asa.o"])])
