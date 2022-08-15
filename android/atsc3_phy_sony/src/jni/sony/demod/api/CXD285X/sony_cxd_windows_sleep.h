/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2015/04/02
  Modification ID : b7d3fbfff615b33d0612092777b65e338801de65
------------------------------------------------------------------------------*/
/**
 @file  sony_cxd_windows_sleep.h

 @brief The Windows specific sleep function definitions.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_WINDOWS_SLEEP_H
#define sony_cxd_WINDOWS_SLEEP_H

/*------------------------------------------------------------------------------
 Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief High precision sleep function for Windows PC.
        This function ensures that sleeping time is not shorter than specified time.

 @param sleepTimeMs Sleep time in ms.

 @return 1 if successful. 0 if failed.
*/
int sony_cxd_windows_Sleep (unsigned long sleepTimeMs);

/**
 @brief This function improves timer accuracy by setting the timer resolution to the
        minimum supported by the Windows PC.

        This function should be called at the start of the application.
        If this function is used, ::sony_cxd_windows_RestoreTimeResolution should be
        called at the end of the application.

 @return 1 if successful. 0 if failed.
*/
int sony_cxd_windows_ImproveTimeResolution (void);

/**
 @brief This function restores the time resolution setting configured
        by ::sony_cxd_windows_ImproveTimeResolution.

 @return 1 if successful. 0 if failed.
*/
int sony_cxd_windows_RestoreTimeResolution (void);

#endif /* sony_cxd_WINDOWS_SLEEP_H */
