// Standard
#ifndef H_STANDARD
	#include "stdlib.h"
	#include "stdio.h"
	#include "string.h"
	#define H_STANDARD
#endif
// Project
#ifndef H_CONSTS
	#include "consts.h"
#endif

#ifndef C_COMMON
	#include "common.c"
#endif

#ifndef H_STRUCTS
	#include "structs.h"
#endif
// Tasks
#include "1.c"
#include "2.c"
#include "3.c"
#include "4.c"


int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf(MSG_NOT_ENOUGH_ARGS);
		return 1;
	}

	printf(MSG_TASK, argv[1]);

    // Decide which task to run

	if (strcmp(argv[1], "1") == 0) {
		task_1();
	} else
	if (strcmp(argv[1], "2") == 0) {
		task_2();
	} else 
	if (strcmp(argv[1], "3") == 0) {
		task_3();
	} else
	if (strcmp(argv[1], "4") == 0) {
		task_4();
	} else {
		printf(MSG_UNKNOWN_ARG, argv[1]);
	}

	BSOD();

    return 0;
}
