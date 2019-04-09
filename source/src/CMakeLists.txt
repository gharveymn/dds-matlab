CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(dxtmex CXX)

SET(DIRECTXTEX_INCLUDE_DIR ../lib/DirectXTex/DirectXTex)
INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR} ${DIRECTXTEX_INCLUDE_DIR})

SET(SOURCE_FILES
		${DIRECTXTEX_INCLUDE_DIR}/DirectXTex.h dxtmex_maps.hpp dxtmex.cpp
		dxtmex_mexerror.cpp dxtmex_mexerror.hpp dxtmex_pixel.hpp dxtmex_pixel.cpp dxtmex_mexutils.cpp dxtmex_mexutils.hpp dxtmex_utils.cpp dxtmex_utils.hpp dxtmex_dxtimagearray.cpp dxtmex_dxtimagearray.hpp dxtmex_maps.cpp dxtmex_dxtimage.cpp dxtmex_flags.hpp)

ADD_LIBRARY(dxtmex.mexw64 ${SOURCE_FILES})
