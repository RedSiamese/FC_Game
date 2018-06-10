from distutils.core import setup, Extension
MOD = 'fcgame'
setup(name=MOD, ext_modules=[Extension(MOD, sources=['FC_PYTHON.cpp',
'Simulator\FC_camera.cpp',
'Simulator\FC_car.cpp',
'Simulator\FC_env.cpp',
'Simulator\FC_map.cpp',
'Simulator\FC_math.cpp',
'Simulator\FC_thread_show.cpp',
'FC\FC_IMAGE.cpp',
'FC\FC_MATH.cpp'])])