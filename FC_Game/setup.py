from distutils.core import setup, Extension
MOD = 'fcgame'
setup(name=MOD, ext_modules=[Extension(MOD, sources=['FC_PYTHON.cpp'])])