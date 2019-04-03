/** mlerrorutils.c
 * Defines error and warning utility functions for easier
 * output of error information.
 *
 * Copyright © 2018 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "mex.h"
#include "dxtmex_mexerror.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>

#ifndef _WIN32
#  include <string.h>
#else
HRESULT hres = 0;
#endif

#include "dxtmex_mexerror.hpp"

#define VALUE_AS_STRING(value) #value
#define EXPAND_AS_STRING(num) VALUE_AS_STRING(num)

#define MEU_SEVERITY_USER_STRING "[USER]"
#define MEU_SEVERITY_INTERNAL_STRING "[INTERNAL]"
#define MEU_SEVERITY_SYSTEM_STRING "[SYSTEM]"
#define MEU_SEVERITY_CORRUPTION_STRING "[CORRUPTION]"
#define MEU_SEVERITY_FATAL_STRING "[FATAL]"

#define MEU_LIBRARY_NAME_SIZE 31
#define MEU_ID_SIZE 95
#define MEU_ID_BUFFER_SIZE 128
#define MEU_ERROR_SEVERITY_SIZE 64
#define MEU_FILE_NAME_SIZE 260
#define MEU_ERROR_STRING_SIZE 2048
#define MEU_SYSTEM_ERROR_BUFFER_SIZE 1100
#define MEU_SYSTEM_ERROR_STRING_SIZE 1024 /* size of POSIX error buffer size */
#define MEU_MATLAB_HELP_MESSAGE_SIZE 512
#define MEU_FULL_MESSAGE_SIZE 4096 /* four times the size of max POSIX error buffer size */

#define MEU_ID_FORMAT "%." EXPAND_AS_STRING(MEU_LIBRARY_NAME_SIZE)"s:%." EXPAND_AS_STRING(MEU_ID_SIZE)"s"

#define MEU_ERROR_MESSAGE_FORMAT "<strong>%." EXPAND_AS_STRING(MEU_ID_BUFFER_SIZE)"s</strong> in %." EXPAND_AS_STRING(MEU_FILE_NAME_SIZE)"s, at line %d.\n%." EXPAND_AS_STRING(MEU_ERROR_SEVERITY_SIZE)"s\n\n%." EXPAND_AS_STRING(MEU_ERROR_STRING_SIZE)"s\n%." EXPAND_AS_STRING(MEU_SYSTEM_ERROR_BUFFER_SIZE)"s%." EXPAND_AS_STRING(MEU_MATLAB_HELP_MESSAGE_SIZE)"s"

#define MEU_WARN_MESSAGE_FORMAT "%." EXPAND_AS_STRING(MEU_ERROR_STRING_SIZE)"s%." EXPAND_AS_STRING(MEU_MATLAB_HELP_MESSAGE_SIZE)"s"

void (*MEXError::error_callback)(unsigned int) = nullptr;
void (*MEXError::warning_callback)() = nullptr;

const char* MEXError::error_help_message = "";
const char* MEXError::warning_help_message = "";

namespace
{
	/**
	 * Writes out the severity string.
	 *
	 * @param buffer A preallocated buffer. Should be size MEU_ERROR_SEVERITY_SIZE.
	 * @param error_severity The error severity bitmask.
	 */
	void WriteSeverityString(char* buffer, unsigned int error_severity)
	{
		char* buffer_iter = buffer;

		strcpy(buffer_iter, "Severity level: ");
		buffer_iter += strlen(buffer_iter);

		if(error_severity & MEU_SEVERITY_USER)
		{
			strcpy(buffer_iter, MEU_SEVERITY_USER_STRING);
			buffer_iter += strlen(MEU_SEVERITY_USER_STRING);
		}

		if(error_severity & MEU_SEVERITY_INTERNAL)
		{
			strcpy(buffer_iter, MEU_SEVERITY_INTERNAL_STRING);
			buffer_iter += strlen(MEU_SEVERITY_INTERNAL_STRING);
		}

		if(error_severity & MEU_SEVERITY_SYSTEM)
		{
			strcpy(buffer_iter, MEU_SEVERITY_SYSTEM_STRING);
			buffer_iter += strlen(MEU_SEVERITY_SYSTEM_STRING);
		}

		if(error_severity & MEU_SEVERITY_CORRUPTION)
		{
			strcpy(buffer_iter, MEU_SEVERITY_CORRUPTION_STRING);
			buffer_iter += strlen(MEU_SEVERITY_CORRUPTION_STRING);
		}

		if(error_severity & MEU_SEVERITY_FATAL)
		{
			strcpy(buffer_iter, MEU_SEVERITY_FATAL_STRING);
		}

	}


	/**
	 * Writes out the system error string.
	 *
	 * @param buffer A preallocated buffer. Should be size MEU_SYSTEM_ERROR_STRING_SIZE.
	 * @param error_code The system error code returned from GetLastError() or errno.
	 */
	void WriteSystemErrorString(char* buffer, unsigned int error_severity)
	{
		char* inner_buffer = buffer;

#ifdef _WIN32
		if(error_severity & MEU_ERRNO)
		{
			sprintf(buffer, "System error code 0x%d: ", errno);
			inner_buffer += strlen(buffer);

			strerror_s(inner_buffer, MEU_SYSTEM_ERROR_STRING_SIZE, errno);
		}
		else if(error_severity & MEU_SEVERITY_HRESULT)
		{
			sprintf(buffer, "System error code 0x%lX: ", hres);
			inner_buffer += strlen(buffer);

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, (DWORD)hres, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), inner_buffer, MEU_SYSTEM_ERROR_STRING_SIZE, nullptr);
		}
		else
		{
			sprintf(buffer, "System error code 0x%lX: ", GetLastError());
			inner_buffer += strlen(buffer);

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), inner_buffer, MEU_SYSTEM_ERROR_STRING_SIZE, nullptr);
		}
#else

		/* we use errno in any case */

		sprintf(buffer, "System error code 0x%d: ", errno);
		inner_buffer += strlen(buffer);

#  if(((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE) || defined(__APPLE__))
		/* XSI-compliant version */
		strerror_r(errno, inner_buffer, MEU_SYSTEM_ERROR_STRING_SIZE);
#  else
		/* GNU-specific */
		strcpy(inner_buffer, strerror_r(errno, inner_buffer, MEU_SYSTEM_ERROR_STRING_SIZE));
#  endif
#endif

		*(buffer + strlen(buffer)) = '\n';

	}
}

void MEXError::PrintMexError(const char* file_name, int line, unsigned int error_severity, const char* error_id, const char* error_message, ...)
{
	
	va_list va;
	
	char full_message[MEU_FULL_MESSAGE_SIZE] = {0};
	char error_message_buffer[MEU_ERROR_STRING_SIZE] = {0};
	char error_severity_buffer[MEU_ERROR_SEVERITY_SIZE] = {0};
	char id_buffer[MEU_ID_BUFFER_SIZE] = {0};
	char system_error_string_buffer[MEU_SYSTEM_ERROR_STRING_SIZE] = {0};
	
	if(error_severity & (MEU_SEVERITY_SYSTEM|MEU_SEVERITY_HRESULT))
	{
		WriteSystemErrorString(system_error_string_buffer, error_severity);
	}
	
	va_start(va, error_message);
	vsprintf(error_message_buffer, error_message, va);
	va_end(va);
	
	WriteSeverityString(error_severity_buffer, error_severity);
	
	sprintf(id_buffer, MEU_ID_FORMAT, MEXError::g_library_name, error_id);
	sprintf(full_message, MEU_ERROR_MESSAGE_FORMAT, error_id, file_name, line, error_severity_buffer, error_message_buffer, system_error_string_buffer, MEXError::error_help_message);
	
	if(MEXError::error_callback != nullptr)
	{
		MEXError::error_callback(error_severity);
	}
	mexErrMsgIdAndTxt(id_buffer, full_message);
	
}


void MEXError::PrintMexWarning(const char* warn_id, const char* warn_message, ...)
{
	va_list va;
	char full_message[MEU_FULL_MESSAGE_SIZE] = {0};
	char message_prebuffer[MEU_ERROR_STRING_SIZE] = {0};
	char id_buffer[MEU_ID_BUFFER_SIZE] = {0};
	
	va_start(va, warn_message);
	vsprintf(message_prebuffer, warn_message, va);
	va_end(va);
	
	sprintf(id_buffer, MEU_ID_FORMAT, MEXError::g_library_name, warn_id);
	sprintf(full_message, MEU_WARN_MESSAGE_FORMAT, message_prebuffer, MEXError::warning_help_message);

	if(MEXError::warning_callback != nullptr)
	{
		MEXError::warning_callback();
	}
	mexWarnMsgIdAndTxt(id_buffer, full_message);
}
