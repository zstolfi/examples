#pragma once
#include <stdio.h>

int error(char msg[]) {
	fprintf(stderr, "Error: %s\n", msg);
	return 1;
}