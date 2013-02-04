#include "graphics_lg1.h"

graphics_lg1::graphics_lg1()
{
	pm = new unsigned char[1024 * 768];
}

graphics_lg1::~graphics_lg1()
{
	delete [] pm;
}
