/*------------------------------------------------------------------------------
  Copyright 2014 Sony Corporation

  Last Updated    : 2014/11/27
  Modification ID : c385cfc07211cb751200038a6e4232fe8b2f8cb5
------------------------------------------------------------------------------*/
/**
 @file  sony_stdlib.h

 @brief C standard lib function aliases.
*/
/*----------------------------------------------------------------------------*/

#ifndef SONY_STDLIB_H
#define SONY_STDLIB_H

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
#define sony_memcpy  memcpy

/**
 @brief Alias for memset.
*/
#define sony_memset  memset

#endif /* SONY_STDLIB_H */
