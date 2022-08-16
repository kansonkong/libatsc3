#ifndef __SMSAPI_MACROS_H__
#define __SMSAPI_MACROS_H__

#define SMS_MIN(a,b)    (((a)<(b))?(a):(b))
#define SMS_MAX(a,b)    (((a)>(b))?(a):(b))

/* Smsapi LOG */
#define SMSAPI_LOG     printk


#endif // __SMSAPI_MACROS_H__
