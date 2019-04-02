CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(dds-matlab CXX)

INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR} headers)

SET(SOURCE_FILES directxtex.h directxtex.inl
		ddsmex_maps.hpp ddsmex.cpp
				ddsmex_mexerror.cpp ddsmex_mexerror.hpp ddsfmt.hpp ddsfmt.cpp ddsmex_mexutils.cpp ddsmex_mexutils.hpp ddsmex_utils.cpp ddsmex_utils.hpp ddsmex_dds.cpp ddsmex_dds.hpp ddsmex_maps.cpp)

ADD_LIBRARY(ddsmex.mexw64 ${SOURCE_FILES})
