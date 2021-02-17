#ifndef INC_SRT_HAICRYPT_LOG_H
#define INC_SRT_HAICRYPT_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAICRYPT_DECLARE_LOG_DISPATCHER(LOGLEVEL) \
    int HaiCrypt_LogF_##LOGLEVEL ( const char* file, int line, const char* function, const char* format, ...)

// Now declare all dispatcher functions

HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_DEBUG);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_NOTICE);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_INFO);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_WARNING);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_ERR);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_CRIT);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_ALERT);
HAICRYPT_DECLARE_LOG_DISPATCHER(LOG_EMERG);

#define HCRYPT_LOG_INIT()
#define HCRYPT_LOG_EXIT()
// #define HCRYPT_LOG(lvl, fmt, ...) HaiCrypt_LogF_##lvl (__FILE__, __LINE__, __FUNCTION__, fmt, __VA_ARGS__)
#define HCRYPT_LOG(lvl, fmt, ...) printf("%-24.24s:%4d:ERROR:",__FILENAME__,__LINE__); printf(__VA_ARGS__); printf("%s%s","\r","\n"); 

#if ENABLE_HAICRYPT_LOGGING == 2
#define HCRYPT_DEV 1
#endif

#ifdef __cplusplus
}
#endif

#endif // macroguard
