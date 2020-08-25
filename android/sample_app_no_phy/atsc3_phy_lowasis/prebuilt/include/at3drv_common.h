/*
	at3drv_common.h

	for Project Atlas

	Define common tools

	Digital STREAM Labs, Inc. 2017
	Copyright © 2017, 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3DRV_COMMON_H__
#define __AT3DRV_COMMON_H__

//============================================================================

#ifdef __AT3_COMMON_H__
  #error at3drv_common.h should be included before at3_common.h
/*
	AT3_ErrString 의 재정의 기능 때문에 at3drv를 사용하는 곳이라면 at3drv_common.h 가 먼저 포함되어야 함.
*/
#endif


#define __STDC_FORMAT_MACROS
	// this should be defined before inttypes.h inclusion for use PRIu64, PRIx64, ...

#ifdef AT3DRV_BUILD_CONFIG
	// AT3DRV_BUILD_CONFIG should be defined only when at3drv library is built from source.
	#include "at3drv_config.h"
#endif

#ifdef AT3DRV_USER_CONFIG
	#include "at3drv_userconfig.h"
#endif


// override error string
/*
	at3base 에서 제공하는 에러 스트링은 At3ErrString().
	at3drv 에서는 libusb 에러 코드까지 문자열 변환을 지원하기 위해 이를 더 확장해서 사용함.
	확장된 api는 AT3DRV_ErrString().

	대부분의 소스 코드 내에서는 이미 AT3_ErrString()를 이용하고 있으므로 (최초 at3drv 개발시의 api였음)
	이것을 #define 하여 AT3DRV_ErrString()으로 교체하여 동작하게 하고 있음.

	이게 제대로 동작하려면 at3base의 at3_type.h 가 포함되기 이전에
	맨 먼저 이 at3drv_common.h 가 포함되도록 하여 AT3_ErrString을 먼저 정의 하도록 해야 한다.
*/
#ifdef AT3_ErrString
  #error AT3_ErrString is already defined?
  // 에러. at3_type.h가 (아마도 at3_common.h 를 통해서) 먼저 포함되었음.
  // 대책: at3drv 에서는 이 헤더 (at3drv_common.h)를 먼저 포함시켜야 함.
#endif
#define AT3_ErrString AT3DRV_ErrString

#include "at3_common.h"
#include "at3drv_version.h"


// include standard c/c++ header here..


// at3drv 는 atsc3.0 지원을 1차 목표로 작성된 것이므로 atsc 관련 std 헤더는 기본 포함시킨다.
#include "at3_atscstd.h"


// include common vendor/3rd-party header here..
#ifdef AT3DRV_BUILD_CONFIG
	#include "libusb.h"
#endif

//============================================================================

/*
	linkage type of public api
*/

#define AT3DRVAPI AT3STDAPI



//============================================================================

AT3DRVAPI const char *AT3DRV_ErrString(AT3RESULT r);



#endif // __AT3DRV_COMMON_H__
