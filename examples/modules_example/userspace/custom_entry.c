#include <unistd.h>	//For _exit() definition

/*
 * In case we want to manually define out entry/exit points, we simply
 * compile with :
 *
 * gcc --entry [fxn] -nostartfiles
 *
 * So long as the function has a _exit() we can safely use it to enter
 * (and eventually terminate) the program. If we omit it, we SEGFAULT.
 */

int test()
{
	write(1, "hello!\n", 7);
	_exit(0);
}
