/**
 * \file
 *
 * \brief Application implement
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/*
 * FEATURE SWITCHES
 */
#define ENABLE_WDT                 (1)

/*
 * INCLUDES
 */
#include "atmel_start.h"
#include "atmel_start_pins.h"

#include "hal_io.h"
#include "hal_rtos.h"

#include "FreeRTOS.h"
#include "task.h"

#if (ENABLE_WDT == 1)
  #include "watchdog.h"
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <time.h>

/*
 * MACROS
 */
#define TASK_HD_STACK_SIZE         (256 / sizeof(portSTACK_TYPE))
#define TASK_HD_PRIORITY           (tskIDLE_PRIORITY + 3)
#define TASK_HD_PERIODICITY_MS     (1000U)

#if (ENABLE_WDT == 1)
  /* TODO: Decide if watchdog task needs to have less priority in order to detect infinite loops in high priority tasks, however, this contradicts with the rate monotonic scheduling model */
  #define TASK_WDT_STACK_SIZE        (256 / sizeof(portSTACK_TYPE))
  #define TASK_WDT_PRIORITY          (tskIDLE_PRIORITY + 2)
  #define TASK_WDT_PERIODICITY_MS    WATCHDOG_REFRESH_RATE
#endif

/*
 * VARIABLE DECLARATIONS
 */
static TaskHandle_t      xCreatedHelloDottTask;

#if (ENABLE_WDT == 1)
static TaskHandle_t      xCreatedWatchdogTask;
#endif

/*
 * FUNCTION DECLARATIONS
 */
static void str_write(const char *s);

static void task_helloDott_create(void);
static void task_helloDott(void *p);

#if (ENABLE_WDT == 1)
static void task_watchdog_create(void);
static void task_watchdog(void *p);
#endif

static void tasks_run(void);

/*
 * FUNCTION DEFINITIONS
 */

/**
 * \brief Write string to console
 */
static void str_write(const char *s)
{
	io_write(&EDBG_COM.io, (const uint8_t *)s, strlen(s));
}

#if (ENABLE_WDT == 1)

/** 
 * \brief Create the task Watchdog
 */
static void task_watchdog_create(void)
{
	BaseType_t xReturned;
	
	/* Initialize watchdog hardware */
	vWDTinit();
	
	xReturned = xTaskCreate(task_watchdog,
	"Watchdog",
	TASK_WDT_STACK_SIZE,
	NULL,
	TASK_WDT_PRIORITY,
	&xCreatedWatchdogTask);
	
	if(xReturned != pdPASS)
	{
		/* TODO: Decide maybe if having an operating WDT is crucial then reset the code with an infinite loop here as well */
		
		/* if something goes wrong, stop the hardware watchdog to avoid resets */
		vWDTstop();
	}
	else
	{
		/* All good! */
	}
}


/** 
 * \brief Periodically refresh the WDT
 */
static void task_watchdog(void *p)
{
	for (;;)
	{
		/* ACTIVITY LIST IN TASK WATCHDOG! */
		vWDTrefresh();		
		
		/* END ACTIVITIES */
		
		os_sleep(TASK_WDT_PERIODICITY_MS);
	}
	
}

#endif

/** 
 * \brief Create the task HelloDott!
 */
static void task_helloDott_create(void)
{
	BaseType_t xReturned;

    xReturned = xTaskCreate(task_helloDott,
	                        "Hello Dott!", 
							TASK_HD_STACK_SIZE, 
							NULL, 
							TASK_HD_PRIORITY, 
							&xCreatedHelloDottTask);
	
	if(xReturned != pdPASS)
	{
		while (1)
		{
			/* Go into an infinite loop, let watchdog reset! */
		}
	}
	else
	{
		/* All good! */
	}
}

/** 
 * \brief Print a Hello Dott! Message every second
 */
static void task_helloDott(void *p)
{
	for (;;) 
	{
		/* ACTIVITY LIST IN TASK HELLODOTT! */
		str_write("Hello Dott!\r\n");
		
		/* END ACTIVITIES */
		
		os_sleep(TASK_HD_PERIODICITY_MS);
	}
}

/** 
 * \brief Start the Scheduler
 */
static void tasks_run(void)
{
	vTaskStartScheduler();
}

int main(void)
{
	/* Init Peripherals */
	atmel_start_init();

	/* Init tasks */
	task_helloDott_create();

#if (ENABLE_WDT == 1)
	task_watchdog_create();
#endif
	
	/* Start the scheduler */
	tasks_run();

	return 0;
}