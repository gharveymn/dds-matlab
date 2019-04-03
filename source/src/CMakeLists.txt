CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(dxtmex CXX)

INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR} headers)

SET(SOURCE_FILES directxtex.h directxtex.inl
		dxtmex_maps.hpp dxtmex.cpp
		dxtmex_mexerror.cpp dxtmex_mexerror.hpp dxtfmt.hpp dxtfmt.cpp dxtmex_mexutils.cpp dxtmex_mexutils.hpp dxtmex_utils.cpp dxtmex_utils.hpp dxtmex_dxtimage.cpp dxtmex_dxtimage.hpp dxtmex_maps.cpp)

ADD_LIBRARY(dxtmex.mexw64 ${SOURCE_FILES})
