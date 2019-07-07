/**
 * \file
 *
 * \brief Hardware Watchdog Driver
 */

/*
 * INCLUDES
 */
#include "watchdog.h"

#if (USE_WDT_DRIVER == ON)
  #include "driver_init.h"
  #include "utils.h"
#else
  #include <hri_e54.h>
#endif

/*
 * EXTERNAL FUNCTION DEFINITION
 */
#if (USE_WDT_DRIVER == ON)

void vWDTinit(void)
{
	uint32_t clk_rate;
	uint16_t timeout_period;
	
	clk_rate       = 1000;
	timeout_period = 4096;
	wdt_set_timeout_period(&WDT_0, clk_rate, timeout_period);
	wdt_enable(&WDT_0);
}

void vWDTrefresh(void)
{
	wdt_feed(&WDT_0);
}

void vWDTstop(void)
{
	wdt_disable(&WDT_0);
}

#else /* USE_WDT_DRIVER == 0 */

/* Set the WDT Configuration Register to the following data:
 * WINDOW: 0x0
 * PER:    0xA - 4 second window
 *
 * Datasheet reference:
 * These bits determine the watchdog time-out period as a number of 1.024kHz CLK_WDTOSC clock cycles.
 */
#define WDT_CONFIG_TIMEOUT (0x0A)


void vWDTinit(void)
{
	/* Clear wdt ENABLE  bit to start configuration */
	hri_wdt_clear_CTRLA_ENABLE_bit(WDT);
	
	/* Configure the timeout value to 4 seconds */
	hri_wdt_set_CONFIG_PER_bf(WDT, WDT_CONFIG_TIMEOUT);
	
	/* Set the wdt ENABLE bit to start the watchdog countdown */
	hri_wdt_set_CTRLA_ENABLE_bit(WDT);
}

void vWDTrefresh(void)
{
	
	/*  Datasheet reference: 	
	 * 	The WDT is cleared and a new WDT time-out period is started by writing 0xA5 to the Clear register
	 * 	(CLEAR). Writing any other value than 0xA5 to CLEAR will issue an immediate system reset.
	 */	
	hri_wdt_write_CLEAR_reg(WDT, 0xA5);
}

void vWDTstop(void)
{
	/* set the pin to cancel hardware watchdog operation */
	hri_wdt_clear_CTRLA_ENABLE_bit(WDT);
}

#endif  /* USE_WDT_DRIVER */