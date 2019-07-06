/**
 * \file
 *
 * \brief Hardware Watchdog Driver
 */

/* This determines the periodicity of the watchdog task, defining the watchdog timeout period to 4 seconds, and the refresh rate to 2 seconds */
#define WATCHDOG_REFRESH_RATE  (2000U)

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