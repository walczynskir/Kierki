#pragma once
#include <fstream>


#ifndef tfile
	#ifdef _UNICODE
		#define tfile			std::fstream
	#else
		#define tfile			std::wfstream
	#endif
#endif
