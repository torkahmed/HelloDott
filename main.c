/**
 * \file
 *
 * \brief Main
 *
 */

/*
 * FEATURE SWITCHES
 */
#define ENABLE_WDT                 (ON)

/*
 * INCLUDES
 */
#include "atmel_start.h"
#include "FreeRTOS.h"
#include "task.h"

#include "sercom.h"

#if (ENABLE_WDT == ON)
  #include "watchdog.h"
#endif

/*
 * MACROS
 */
#define TASK_HD_STACK_SIZE         (256 / sizeof(portSTACK_TYPE))
#define TASK_HD_PRIORITY           (tskIDLE_PRIORITY + 3)
#define TASK_HD_PERIODICITY_MS     (1000U)

#if (ENABLE_WDT == ON)
  /* watchdog task needs to have less priority in order to detect infinite loops in high priority tasks */
  #define TASK_WDT_STACK_SIZE        (256 / sizeof(portSTACK_TYPE))
  #define TASK_WDT_PRIORITY          (tskIDLE_PRIORITY + 2)
  #define TASK_WDT_PERIODICITY_MS    WATCHDOG_REFRESH_RATE
#endif

/*
 * VARIABLE DECLARATIONS
 */
static TaskHandle_t      xCreatedHelloDottTask;

#if (ENABLE_WDT == ON)
static TaskHandle_t      xCreatedWatchdogTask;
#endif

/*
 * LOCAL FUNCTION DECLARATIONS
 */

static void task_helloDott_create(void);
static void task_helloDott(void *p);

#if (ENABLE_WDT == ON)
static void task_watchdog_create(void);
static void task_watchdog(void *p);
#endif

static void tasks_run(void);

/*
 * LOCAL FUNCTION DEFINITIONS
 */

#if (ENABLE_WDT == ON)

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
		WriteToConsole("Hello Dott!\r\n");
		
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


/*
 * MAIN
 */

int main(void)
{
	/* Init Peripherals */
	atmel_start_init();
	
	/* Init Sercom */
	InitSercom();

	/* Init tasks */
	task_helloDott_create();

#if (ENABLE_WDT == ON)
	task_watchdog_create();
#endif
	
	/* Start the scheduler */
	tasks_run();

	return 0;
}