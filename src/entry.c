// Standard
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Project
#include "consts.h"


// Helpers

// void BSOD() {
//     __asm {
//         xor ax, ax
//         mov cs, ax
//     }
// }

// End of helpers

// Tasks

// void task_1() {
//     printf(">> Task 1\n");
// }

// void task_2() {
//     printf(">> Task 2\n");
// }

// void task_3() {
//     printf(">> Task 3\n");
// }

// void task_4() {
//     printf(">> Task 4\n");
// }

// End of tasks


int main(int argc, char *argv[]) {
    // if (argc < 2) {
    //     printf(MSG_NOT_ENOUGH_ARGS);
    //     return 1;
    // }

    // printf(MSG_TASK, argv[1]);

    // Get basic sysinfo
    
    printf("fef\n");

    SYSINFO sysinfo;

    memset(&sysinfo, 0, sizeof(sysinfo));
    get_sysinfo(&sysinfo);

    // Decide which task to run

    // if (strcmp(argv[1], "1") == 0) {
    //     task_1();
    // } else
    // if (strcmp(argv[1], "2") == 0) {
    //     task_2();
    // } else 
    // if (strcmp(argv[1], "3") == 0) {
    //     task_3();
    // } else
    // if (strcmp(argv[1], "4") == 0) {
    //     task_4();
    // } else {
    //     printf(MSG_UNKNOWN_ARG, argv[1]);
    // }

    // BSOD();

    return 0;
}
