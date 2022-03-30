#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lwshell/lwshell.h"
#include "events.h"
#include "scheduler.h"

#include "systick.h"
#include "leds.h"
#include "analog_inputs.h"


#include "shell.h"

extern const char app_name[];
extern const char app_version[];

extern uint32_t SystemCoreClock;

static void shell_output(const char *str, struct lwshell *lw);

static void shell_welcome(enum events e, int ticks);
static void shell_input(enum events e, int ticks);

static int32_t shell_cmd_version(int32_t argc, char **argv);
static int32_t shell_cmd_led_set(int32_t argc, char **argv);
static int32_t shell_cmd_analog_input(int32_t argc, char **argv);
static int32_t shell_cmd_diag(int32_t argc, char **argv);

int a2i(char* txt)
{
    int sum, digit, i;
    sum = 0;
    for (i = 0; i < strlen(txt); i++) {
        digit = txt[i] - 0x30;
        sum = (sum * 10) + digit;
    }
    return sum;
}

void shell_init(void)
{
    lwshell_init();
    lwshell_set_output_fn(shell_output);
    lwshell_register_cmd("version", shell_cmd_version, "Print application name and version");
    lwshell_register_cmd("led", shell_cmd_led_set, "Set a led");
    lwshell_register_cmd("ai", shell_cmd_analog_input, "Get an analog input");
    lwshell_register_cmd("diag", shell_cmd_diag, "Diagnostics");

    event_subscribe(EVENT_USB_VCOM_OPENED, shell_welcome);
    event_subscribe(EVENT_USB_VCOM_RX, shell_input);
}

static void shell_output(const char *str, struct lwshell *lw)
{
    printf(str);
}

static void shell_welcome(enum events e, int ticks)
{
    printf("\n\nStarting.... %s : %s\n", app_name, app_version);
    printf("\nSystem clock rate: %ld Hz\n", SystemCoreClock);
}

static void shell_input(enum events e, int ticks)
{
    int i; 
    uint8_t buffer[32];

    for (;;)
    {
        i = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (i == 0)
            return;

        lwshell_input(buffer, i);
    }
}

static int32_t shell_cmd_version(int32_t argc, char **argv)
{
    printf("App: %s : %s\n", app_name, app_version);
    return 0;
}

static int32_t shell_cmd_led_set(int32_t argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: led 1 1\n");
        return 1;
    }

    int led = a2i(argv[1]);
    int state = a2i(argv[2]);

    printf("leds_set(%d, %d)\n", led, state);

    leds_set(led, state);
    return 0;
}

static int32_t shell_cmd_analog_input(int32_t argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ai 0\n");
        return 1;
    }

    int analog_input = a2i(argv[1]);
    if (argv[1][0] == 'x')
    {
        for (int i=0; i<ANALOG_INPUTS; i++)
        {
            printf("Result[%d]:%ld\n", i, analog_inputs_get(i));
        }
        return 0;
    }

    printf("analog_inputs_get(%d)\n", analog_input);
    if (analog_input >= ANALOG_INPUTS)
    {
        printf("!analog_input:%d is not in range [0..%d]\n", analog_input, ANALOG_INPUTS-1);
        return 1;
    }

    uint32_t result = analog_inputs_get(analog_input);
    printf("Result:%ld\n", result);

    return 0;
}

static int32_t shell_cmd_diag(int32_t argc, char **argv)
{
    printf("diag\n");
    printf("-----------\n");
    printf("Scheduler space: %d\n", scheduler_space());
    printf("pwm int per sec: %ld\n", analog_inputs_pwm_rate());
    printf("adc int per sec: %ld\n", analog_inputs_adc_rate());
    printf("idle_missed: %d\n", systick_idle_missed());
    printf("handlers_called_max: %d\n", scheduler_get_diagnostics()->handlers_called_max);
    printf("event_queue_full: %d\n", scheduler_get_diagnostics()->event_queue_full);

    return 0;
}