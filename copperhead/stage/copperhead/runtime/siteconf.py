#! /usr/bin/env python
#
# Configuration file.
# Use Python syntax, e.g.:
# VARIABLE = "value"
# 
# The following information can be recorded:
#
# CXX : path and name of the host c++ compiler, eg: /usr/bin/g++-4.5
#
# CC : path and name of the host c compiler, eg: /usr/bin/gcc
#
# BOOST_INC_DIR : Directory where the Boost include files are found.
#
# BOOST_LIB_DIR : Directory where Boost shared libraries are found.
#
# BOOST_PYTHON_LIBNAME : Name of Boost::Python shared library.
#   NOTE: Boost::Python must be compiled using the same compiler
#   that was used to build your Python.  Strange errors will
#   ensue if this is not true.
# CUDA_INC_DIR : Directory where CUDA include files are found
#
# CUDA_LIB_DIR : Directory where CUDA libraries are found
#
# NP_INC_DIR : Directory where Numpy include files are found.
#
# TBB_INC_DIR : Directory where TBB include files are found
#
# TBB_LIB_DIR : Directory where TBB libraries are found
#
# THRUST_DIR : Directory where Thrust include files are found.
#


BOOST_INC_DIR = None
BOOST_LIB_DIR = None
BOOST_PYTHON_LIBNAME = "boost_python"
CC = "gcc"
CUDA_INC_DIR = "/usr/local/cuda/include"
CUDA_LIB_DIR = "/usr/local/cuda/lib64"
CXX = "g++"
NP_INC_DIR = "/usr/lib/python2.7/dist-packages/numpy/core/include"
TBB_INC_DIR = None
TBB_LIB_DIR = None
THRUST_DIR = "/usr/local/cuda/include/thrust"
