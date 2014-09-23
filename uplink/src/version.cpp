
#ifndef WIN32

#include <string.h>
#include <iostream>

#include "app/globals_defines.h"

int main(int, char **)
{
	std::cout << VERSION_NUMBER << std::endl;
	return 0;
}

#endif
