CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
PROJECT(dds-matlab CXX)

INCLUDE_DIRECTORIES(${MATLAB_INCLUDE_DIR} headers)

SET(SOURCE_FILES	headers/directxtex.h headers/directxtex.inl
				headers/dxgitypedefs.h
		headers/ddsio.h ddsio.cpp
				mlerrorutils.cpp headers/mlerrorutils.h)

ADD_EXECUTABLE(ddsread ${SOURCE_FILES})
