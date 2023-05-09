// Tick1000.h

#ifndef _TICK1000_h
#define _TICK1000_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <hardware/timer.h>
#include <hardware/irq.h>

#define ALARM_NUM 1
#define ALARM_IRQ TIMER_IRQ_1

uint32_t alarmPeriod;

void tick_loop();

int ledval;
static void alarm_in_us_arm(uint32_t delay_us)
{
	uint64_t target = timer_hw->timerawl + delay_us;
	timer_hw->alarm[ALARM_NUM] = (uint32_t)target;
}

static void alarm_irq(void)
{
	hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);
	alarm_in_us_arm(alarmPeriod);
	tick_loop();
}

static void periodForTickLoop(uint32_t delay_us)
{
	alarmPeriod = delay_us;
	hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
	irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
	irq_set_enabled(ALARM_IRQ, true);
	alarm_in_us_arm(delay_us);
}
#endif

