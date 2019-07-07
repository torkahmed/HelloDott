/**
 * \file
 *
 * \brief Hardware Watchdog Driver
 */

/*
 * FEATURE SWITCHES
 */
/* Whether or not to use the WDT standard driver by atmel, the alternative is to write directly into the registers from a lower layer */
#define USE_WDT_DRIVER                 (ON)

/* This determines the periodicity of the watchdog task, defining the watchdog timeout period to 4 seconds, and the refresh rate to 2 seconds */
#define WATCHDOG_REFRESH_RATE  (2000U)


/*
 * FUNCTION DECLARATION
 */
/** 
 * \brief Initialize the watchdog register WDT
 */
void vWDTinit(void);

/** 
 * \brief Periodically refresh the watchdog CLEAR register
 */
void vWDTrefresh(void);

/** 
 * \brief Stop WDT operation by clearing the CTRLA.ENABLE register
 */
void vWDTstop(void);