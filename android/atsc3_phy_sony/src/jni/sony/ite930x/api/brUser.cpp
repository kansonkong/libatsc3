#include "brUser.h"
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#include <SonyPHYAndroid.h>

sem_t sem;

/**
 * Variable of critical section
 */

Dword BrUser_delay (
    IN  Bridge*         bridge,
    IN  Dword           dwMs
) {
    /*
        *  ToDo:  Add code here
        *
        *  //Pseudo code
        *  delay(dwMs);
        *  return (0);
        */
     
	usleep(dwMs * 1000);
    return (BR_ERR_NO_ERROR);
}

Dword BrUser_createCriticalSection (
    void
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
	int res = sem_init(&sem, 0, 1);
    return res;
}

Dword BrUser_deleteCriticalSection (
    void
) {
    /*
     *  ToDo:  Add code here
     *
     *  //Pseudo code
     *  return (0);
     */
	int res = sem_destroy(&sem);
    return res;
}

Dword BrUser_enterCriticalSection (
    IN  Bridge*         bridge
) {
    /*
        *  ToDo:  Add code here
        *
        *  //Pseudo code
        *  return (0);
        */
	int res = sem_wait(&sem);
    return res;
}


Dword BrUser_leaveCriticalSection (
    IN  Bridge*         bridge
) {
    /*
        *  ToDo:  Add code here
        *
        *  //Pseudo code
        *  return (0);
        */
	int res  = sem_post(&sem);
    return res;
}

Dword BrUser_busTx (
    IN  Bridge*         bridge,
    IN  Byte			i2cAddr,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {

	Dword retVal = BR_ERR_NULL_HANDLE_PTR;
    int transferred = -1;

    //jjustman-2022-08-11 - todo - fix me for context handle instead of class static public accessor
    if (SonyPHYAndroid::Libusb_device_handle != NULL) {
        retVal = libusb_bulk_transfer(SonyPHYAndroid::Libusb_device_handle, SonyPHYAndroid::SONY_USB_ENDPOINT_TX, buffer, bufferLength, &transferred, 500);

        if (retVal) {
            _SONY_PHY_ANDROID_ERROR("BrUser_busTx write failed, ep: %d, retVal: %d, transferred: %d", SonyPHYAndroid::SONY_USB_ENDPOINT_TX, retVal, transferred);
        }
    }

    return retVal;
}


Dword BrUser_busRx (
    IN  Bridge*         bridge,
    IN  Byte			i2cAddr,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {

    Dword retVal = BR_ERR_NULL_HANDLE_PTR;
    int transferred = -1;

    //jjustman-2022-08-11 - todo - fix me for context handle instead of class static public accessor
    if (SonyPHYAndroid::Libusb_device_handle != NULL) {
        retVal = libusb_bulk_transfer(SonyPHYAndroid::Libusb_device_handle, SonyPHYAndroid::SONY_USB_ENDPOINT_RX, buffer, bufferLength, &transferred, 500);

        if (retVal) {
            _SONY_PHY_ANDROID_ERROR("BrUser_busRx write failed, ep: %d, retVal: %d, transferred: %d", SonyPHYAndroid::SONY_USB_ENDPOINT_RX, retVal, transferred);
        }
    }

    return retVal;
}

//
//Dword GetTickCount(void)
//{
//#if 0
//    struct timeval currTick;
//    Dword ulRet;
//
//    do_gettimeofday(&currTick);
//    ulRet = currTick.tv_sec;
//    ulRet *= 1000;
//    ulRet += (currTick.tv_usec + 500) / 1000;
//    //printk("GetTickCount = %ld \n", ulRet);
//    return ulRet;
//#endif
//#if 1
//	static struct timeval curr_time;
//	Dword ulRet;
//	do_gettimeofday(&curr_time);
//	ulRet = curr_time.tv_sec*1000 + curr_time.tv_usec/1000;
//	//printk("GetTickCount = %ld \n", ulRet);
//	return ulRet;
//#endif
//}


Dword BrUser_time (
		OUT Dword*			time
) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    //ns -> 9 zeros
    *time = (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

Dword BrUser_time (
) {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	//ns -> 9 zeros
	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

#if 0
Dword BrUser_busRxData (
    IN  Bridge*         bridge,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    return (BR_ERR_NO_ERROR);
}
#endif

