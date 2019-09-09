/*
 * endianess.h
 */
#include <arpa/inet.h>

#ifndef htonll

#define htonll(x) ((((uint64_t)htonl(x)) << 32) + htonl((x) >> 32))

//unsigned long long htonll(unsigned long long v) {
//#define htonll(v)						\
//  (union { unsigned long lv[2]; unsigned long long llv; } u;	\
//    u.lv[0] = htonl(v >> 32); \
//    u.lv[1] = htonl(v & 0xFFFFFFFFULL); \
//    return u.llv; \
//)

#define ntohll(x) ((((uint64_t)ntohl(x)) << 32) + ntohl((x) >> 32))

//unsigned long long ntohll(unsigned long long v) {
//#define ntohll(v)					     \
//  (union { unsigned long lv[2]; unsigned long long llv; } u; \
//    u.llv = v; \
//   return ((unsigned long long)ntohl(u.lv[0]) << 32) | (unsigned long long)ntohl(u.lv[1]); \
//)

#endif
