import os
import ycm_core

flags = ["-I/usr/lib/python3.10/site-packages/numpy/core/include/numpy", "-I/usr/include/python3.10/", "-lpython3.10",  "-shared", "-fPIC"]

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', ]

def FlagsForFile( filename, **kwargs ):
  return {
  'flags': flags,
  'do_cache': True
  }
