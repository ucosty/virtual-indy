#include <assert.h>

#ifdef _DEBUG
	#define ASSERT(x)	assert(x)
	#define DEBUG(x)	do { (x); } while(0)
#else
	#define ASSERT(x)
	#define DEBUG(x)	do { } while(0)
#endif
