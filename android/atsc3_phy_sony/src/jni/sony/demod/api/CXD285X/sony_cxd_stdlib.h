/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2014/11/27
  Modification ID : c385cfc07211cb751200038a6e4232fe8b2f8cb5
------------------------------------------------------------------------------*/
/**
 @file  sony_cxd_stdlib.h

 @brief C standard lib function aliases.
*/
/*----------------------------------------------------------------------------*/

#ifndef sony_cxd_STDLIB_H
#define sony_cxd_STDLIB_H

/*
 PORTING. Please modify if ANCI C standard library is not available.
*/
#include <linux/string.h> 


/*------------------------------------------------------------------------------
 Defines
------------------------------------------------------------------------------*/

/**
 @brief Alias for memcpy.
*/
#define sony_cxd_memcpy  memcpy

/**
 @brief Alias for memset.
*/
#define sony_cxd_memset  memset

#endif /* sony_cxd_STDLIB_H */
