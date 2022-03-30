#include <stdint.h>
#include <stdio.h>


#include "shell.h"
#include "scheduler.h"

uint32_t SystemCoreClock;
const char app_name[] = "sim";
const char app_version[] = "V00R00";


int main (int argc, char *argv[])
{
    printf("Hello\n");
    
    shell_init();



    scheduler_loop();

    return 0;
}