/** mlerrorutils.h
 * Declares functions and macros for mex error utility functions.
 *
 * Copyright © 2018 Gene Harvey
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Windows.h>
   typedef DWORD errcode_T;
   extern HRESULT hres;
#else
   typedef int errcode_T;
#  if(((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE) || defined(__APPLE__))
     /* XSI-compliant version */
     extern int strerror_r(int errnum, char *buf, size_t buflen);
#  else
     /* GNU-specific */
     extern char *strerror_r(int errnum, char *buf, size_t buflen);
#  endif
#endif

#define MEU_SEVERITY_USER        1u << 0u
#define MEU_SEVERITY_INTERNAL    1u << 1u
#define MEU_SEVERITY_SYSTEM      1u << 2u
#define MEU_SEVERITY_CORRUPTION  1u << 3u
#define MEU_SEVERITY_FATAL       1u << 4u
#define MEU_ERRNO                1u << 5u
#define MEU_SEVERITY_HRESULT     1u << 6u

#define MEU_FL __FILE__, __LINE__

namespace MEXError
{
	/**
	 * Prints the specified error in MATLAB. Takes various parameters.
	 *
	 * @param file_name The file name. Only pass __FILE__.
	 * @param line The line number. Only pass __LINE__.
	 * @param error_severity The error severity. pass a bitmask of the MEU_SEVERITY macros.
	 * @param error_code The system error code fetched by either GetLastError() or errno.
	 * @param error_id The identifier for this error which will be appended to "[library name]:".
	 * @param error_message The printf message format associated to the error.
	 * @param ... The error message params in printf style.
	 */
	void PrintMexError(const char* file_name, int line, unsigned int error_severity, const char* error_id, const char* error_message, ...);


	/**
	 * Prints the specified warning message in MATLAB.
	 *
	 * @param warn_id The identifier for this warning which will be appended to "[library name]:".
	 * @param warn_message The printf message format associated to the error.
	 * @param ... The error message params in printf style.
	 */
	void PrintMexWarning(const char* warn_id, const char* warn_message, ...);
	
	extern const char* g_library_name;
	
	extern void (*error_callback)(unsigned int);
	extern void (*warning_callback)();
	
	extern const char* error_help_message;
	extern const char* warning_help_message;
	
}
