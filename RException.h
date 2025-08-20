#pragma once

#include "rstring.h"
#include <fstream>

class RException
{
public:

	RException() {};
	~RException(void) {};

	virtual tstring GetFormattedMsg(void) = 0;
};
