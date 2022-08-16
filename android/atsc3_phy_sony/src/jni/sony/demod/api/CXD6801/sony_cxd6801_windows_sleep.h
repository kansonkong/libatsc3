/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/07/27
  Modification ID : ff73ffebff57618d6a5aa6e2aa9eb009b86060d1
------------------------------------------------------------------------------*/
/**
 @file  sony_cxd6801_windows_sleep.h

 @brief Windows specific sleep function definition.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_CXD6801_WINDOWS_SLEEP_H
#define SONY_CXD6801_WINDOWS_SLEEP_H

/*------------------------------------------------------------------------------
 Function Prototypes
------------------------------------------------------------------------------*/
/**
 @brief High precision sleep function for Windows PC.

        This function ensures that sleeping time is not shorter than specified time.

 @param sleepTimeMs Sleep time in ms.

 @return 1 if successful. 0 if failed.
*/
int sony_cxd6801_windows_Sleep (unsigned long sleepTimeMs);

/**
 @brief This function improves timer accuracy by setting the timer resolution to the
        minimum supported by the Windows PC.

        This function should be called at the start of the application.
        If this function is used, ::sony_cxd6801_windows_RestoreTimeResolution should be called
        at the end of the application.

 @return 1 if successful. 0 if failed.
*/
int sony_cxd6801_windows_ImproveTimeResolution (void);

/**
 @brief This function restores the time resolution setting configured
        by ::sony_cxd6801_windows_ImproveTimeResolution.

 @return 1 if successful. 0 if failed.
*/
int sony_cxd6801_windows_RestoreTimeResolution (void);

#endif /* SONY_CXD6801_WINDOWS_SLEEP_H */
