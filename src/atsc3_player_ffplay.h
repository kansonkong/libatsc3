/*
 * atsc3_player_ffplay.h
 *
 *  Created on: Feb 10, 2019
 *      Author: jjustman
 */
#include <stdio.h>

#ifndef ATSC3_PLAYER_FFPLAY_H_
#define ATSC3_PLAYER_FFPLAY_H_

extern int _PLAYER_FFPLAY_DEBUG_ENABLED;
extern int _PLAYER_FFPLAY_TRACE_ENABLED;

FILE* pipe_create_ffplay();



#define __PLAYER_FFPLAY_ERROR(...)   printf("%s:%d:ERROR: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");
#define __PLAYER_FFPLAY_WARN(...)    printf("%s:%d:WARN: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");
#define __PLAYER_FFPLAY_INFO(...)    printf("%s:%d:INFO: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n");

#define __PLAYER_FFPLAY_DEBUG(...)   if(_PLAYER_FFPLAY_DEBUG_ENABLED) { printf("%s:%d:DEBUG: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n"); }
#define __PLAYER_FFPLAY_TRACE(...)   if(_PLAYER_FFPLAY_TRACE_ENABLED) { printf("%s:%d:TRACE: ",__FILE__,__LINE__); printf(__VA_ARGS__); printf("\n"); }

#endif /* ATSC3_PLAYER_FFPLAY_H_ */
