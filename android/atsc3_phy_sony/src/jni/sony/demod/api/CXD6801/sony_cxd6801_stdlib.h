/*------------------------------------------------------------------------------
  Copyright 2016-2018 Sony Semiconductor Solutions Corporation

  Last Updated    : 2018/09/28
  Modification ID : 1ea04def104d657deb64f0d0b0cae6ef992a9dd8
------------------------------------------------------------------------------*/
/**
 @file  sony_stdlib.h

 @brief C standard lib function replacement configurations.
*/
/*----------------------------------------------------------------------------*/
//#include "sony_cxd_stdlib.h"

#ifndef SONY_CXD6801_STDLIB_H
#define SONY_CXD6801_STDLIB_H

/*
 PORTING. Please modify if ANCI C standard library is not available.
*/
#include <linux/string.h>
//#include <stdlib.h>

/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

/**
 @brief Alias for memcpy.
*/
#define sony_cxd6801_memcpy  memcpy

/**
 @brief Alias for memset.
*/
#define sony_cxd6801_memset  memset

/**
 @brief Alias for abs.
*/
#define sony_cxd6801_abs     abs

#endif /* SONY_CXD6801_STDLIB_H */
