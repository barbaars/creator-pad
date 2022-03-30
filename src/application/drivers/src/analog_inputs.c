#include "analog_inputs.h"

#include "NUC123.h"

#include "io_map.h"

#include "trace.h"
#include "usb_out.h"


volatile uint32_t pwm_int_counter;
volatile uint32_t adc_int_counter;

volatile uint32_t pwm_int_couter_delta;
volatile uint32_t adc_int_couter_delta;

// Double buffered trace
trace trace_primary[ANALOG_INPUTS];
trace trace_secondary[ANALOG_INPUTS];

trace *trace_active[ANALOG_INPUTS];
trace *trace_backup[ANALOG_INPUTS];

rbuff trace_transmit_queue;
volatile uint32_t trace_transmit_queue_memory[32];


static volatile uint32_t analog_inputs_value[ANALOG_INPUTS];

static volatile int analog_inputs_channel;

static void analog_inputs_store(void);
static void analog_inputs_next(void);


void ADC_IRQHandler(void)
{
    if(ADC_GET_INT_FLAG(ADC, ADC_CMP0_INT) != 0)
    {
        ADC_CLR_INT_FLAG(ADC, ADC_CMP0_INT);     /* clear the A/D compare flag 0 */
    }

    if(ADC_GET_INT_FLAG(ADC, ADC_CMP1_INT) != 0)
    {
        ADC_CLR_INT_FLAG(ADC, ADC_CMP1_INT);     /* clear the A/D compare flag 1 */
    }

    if(ADC_GET_INT_FLAG(ADC, ADC_ADF_INT) != 0)
    {
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);     /* clear the A/D convert complete flag */
        analog_inputs_store();
        adc_int_counter++;
    }
}

void PWMA_IRQHandler(void)
{
    PWM_ClearADCTriggerFlag(PWMA, 0, 0);
    PWM_ClearPeriodIntFlag(PWMA, 0);

    analog_inputs_next();

    pwm_int_counter++;
}

static inline void analog_inputs_channel_save(int channel, uint32_t value)
{
    analog_inputs_value[channel] = value;
    int result = trace_put(trace_active[analog_inputs_channel], value);
    if (result && trace_backup[analog_inputs_channel]->state == TRACE_IDLE)
    {
        // Add trace to transmit queue 
        if (rbuff_full(&trace_transmit_queue))
        {
            trace *trash;
            int result = rbuff_get_word(&trace_transmit_queue, (uint32_t *)trash);
            if (result)
            {
                trace_reset(trash);
            }
        }
        rbuff_put_word(&trace_transmit_queue, (uint32_t)trace_active[analog_inputs_channel]);

        trace_swap(&trace_active[analog_inputs_channel], &trace_backup[analog_inputs_channel]);
    }
}

static void analog_inputs_store(void)
{
    if (analog_inputs_channel < 8)
    {
        analog_inputs_channel_save(analog_inputs_channel, ADC_GET_CONVERSION_DATA(ADC, 0));
    }
    else
    {
        analog_inputs_channel_save(analog_inputs_channel, ADC_GET_CONVERSION_DATA(ADC, analog_inputs_channel-7));
    }
}

static void analog_inputs_next(void)
{
    if (++analog_inputs_channel >= ANALOG_INPUTS)
        analog_inputs_channel = 0;

    if (analog_inputs_channel < 8)
    {
        uint32_t mux_selector = analog_inputs_channel;

        ANALOG_MUX_S0 = mux_selector      & 0x1;
        ANALOG_MUX_S1 = (mux_selector>>1) & 0x1;
        ANALOG_MUX_S2 = (mux_selector>>2) & 0x1;

        ADC_Open(ADC, 0, ADC_ADCR_ADMD_SINGLE, 1);
    }
    else
    {
        ADC_Open(ADC, 0, ADC_ADCR_ADMD_SINGLE, 2 << (analog_inputs_channel-8));
    }
}

static void analog_inputs_trace_init(void)
{
    for (int i=0; i<ANALOG_INPUTS; i++)
    {
        trace_init(&trace_primary[i], i, 120);
        trace_init(&trace_secondary[i], i, 120);

        trace_active[i] = &trace_primary[i];
        trace_backup[i] = &trace_secondary[i];
    }

    rbuff_init(&trace_transmit_queue, (void *)trace_transmit_queue_memory, 32);

}

void analog_inputs_init(void)
{
    analog_inputs_trace_init();

    ADC_POWER_ON(ADC);

    /* Configure the hardware trigger condition and enable hardware trigger; PWM trigger delay: (4*10) system clock cycles*/
    ADC_EnableHWTrigger(ADC, ADC_ADCR_TRGS_PWM, NULL);

    /* Center-aligned type */
    PWM_SET_ALIGNED_TYPE(PWMA, 0x1, PWM_CENTER_ALIGNED);
    PWMA->PCR |= PWM_PCR_CH0MOD_Msk; // Auto reload

    /* Input 12 MHz */
    /* Clock prescaler */
    PWM_SET_PRESCALER(PWMA, PWM_CH0, 120);

    /* 12 MHz / 120 = 100 kHz*/

    /* Timer 0 divisor */
    PWM_SET_DIVIDER(PWMA, PWM_CH0, PWM_CLK_DIV_1);
    /* PWM counter value up and down 2*50=100 = 1kHz = 1 interrupt every 1ms */
    PWM_SET_CNR(PWMA, PWM_CH0, 50);
    /* PWM compare value */
    PWM_SET_CMR(PWMA, PWM_CH0, 20);

    /* Enable PWM0 to trigger ADC */
    PWM_EnableADCTrigger(PWMA, PWM_CH0, 0);

    /* Enable PWM interupt */
    PWM_EnablePeriodInt(PWMA, PWM_CH0,  PWM_PERIOD_INT_UNDERFLOW);
    NVIC_EnableIRQ(PWMA_IRQn);

    /* 400 kHz / 36 samples per conversion = 1 coversion takes 90 us */
    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);

    /* Setup first conversion */
    analog_inputs_channel = ANALOG_INPUTS;
    analog_inputs_next();

    /* Start PWM module */
    PWM_Start(PWMA, 0x1);

    event_subscribe(EVENT_CYCLIC_1, analog_inputs_task);      // Every tick
    event_subscribe(EVENT_CYCLIC_7, analog_inputs_diag_task); // Every 1 second
}

uint32_t analog_inputs_get(analog_inputs_t input)
{
    return analog_inputs_value[input];
}

void analog_inputs_task(enum events event, int ticks)
{
    static trace *tr = NULL;

    if (usb_out_ready())
    {
        if (tr != NULL)
        {
            trace_reset(tr);
            tr = NULL;
        }

        int result = rbuff_get_word(&trace_transmit_queue, (uint32_t *)&tr);
        if (result == 0)
        {
            usb_out_send((uint8_t *)tr, sizeof(trace));
        }
    }
}

void analog_inputs_diag_task(enum events event,int ticks)
{
    static uint32_t pwm_memory;
    static uint32_t adc_memory;

    pwm_int_couter_delta = pwm_int_counter - pwm_memory;
    pwm_memory = pwm_int_counter;

    adc_int_couter_delta = adc_int_counter - adc_memory;
    adc_memory = adc_int_counter;
}

uint32_t analog_inputs_adc_rate(void)
{
    return adc_int_couter_delta;
}

uint32_t analog_inputs_pwm_rate(void)
{
    return pwm_int_couter_delta;
}

