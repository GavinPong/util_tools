#include <stdio.h>
#include <string.h>
#include "cross_platform.h"

int main()
{
	printf("wellcomm %s\n", __FUNCTION__);

	Msleep(2 * 1000);
	return 0;
}