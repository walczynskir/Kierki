#pragma once

// normal and unicode strings
#include <string>
#include <sstream>

#ifndef tstring
	#ifdef _UNICODE
		#define tstring			std::wstring
	#else
		#define tstring			std::string		
	#endif
#endif

#ifndef tstringstream
	#ifdef _UNICODE
		#define tstringstream	std::wstringstream
	#else
		#define tstringstream	std::stringstream
	#endif
#endif
