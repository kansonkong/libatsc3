#include "brUser.h"
//#include "windows.h"
//#include "bus\i2cimpl.h"
//#include "bus\usb2impl.h"
#include <unistd.h>
//#include "../Endeavour-jni.h"

#include <semaphore.h>

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

extern jlong busTx(Dword bufferLength, Byte* buffer);
Dword BrUser_busTx (
    IN  Bridge*         bridge,
    IN  Byte			i2cAddr,
    IN  Dword           bufferLength,
    IN  Byte*           buffer
) {
    /*
        *  ToDo:  Add code here
        *
        *  //Pseudo code
        *  short i;
        *
        *  start();
        *  write_i2c(uc2WireAddr);
        *  ack();
        *  for (i = 0; i < bufferLength; i++) {
        *      write_i2c(*(ucpBuffer + i));
        *      ack();
        *  }
        *  stop();
        *
        *  // If no error happened return 0, else return error code.
        *  return (0);
        */

	/*
    Dword                   error = BR_ERR_NO_ERROR;        
    Endeavour* pendeavour = (Endeavour *)bridge;
	Dword       i;
    //if (pendeavour->ctrlBus == BUS_I2C)
    //    error = I2c_writeControlBus(bridge, i2cAddr, bufferLength, buffer);
    //else if (pendeavour->ctrlBus == BUS_USB)
    //    error = Usb2_writeControlBus(bridge, bufferLength, buffer);

	if (pendeavour->ctrlBus == BUS_I2C)
	{
		 for (i = 0; i < IT9300User_RETRY_I2C_MAX_LIMIT; i++) 
		 {
			error = I2c_writeControlBus(bridge, i2cAddr, bufferLength, buffer);
			if (error == 0) return error;
			BrUser_delay (bridge, 1);
		}
	}
	else if (pendeavour->ctrlBus == BUS_USB)
	{
		 for (i = 0; i < IT9300User_RETRY_USB_MAX_LIMIT; i++) 
		 {
			error = Usb2_writeControlBus(bridge, bufferLength, buffer);
			if (error == 0) return error;
			BrUser_delay (bridge, 1);
		}
	}
    
    return (error);*/
	return busTx(bufferLength, buffer);
}

extern jlong busRx(Dword bufferLength, Byte* buffer);
Dword BrUser_busRx (
    IN  Bridge*         bridge,
    IN  Byte			i2cAddr,
    IN  Dword           bufferLength,
    OUT Byte*           buffer
) {
    /*
        *  ToDo:  Add code here
        *
        *  //Pseudo code
        *  short i;
        *
        *  start();
        *  write_i2c(uc2WireAddr | 0x01);
        *  ack();
        *  for (i = 0; i < bufferLength - 1; i++) {
        *      read_i2c(*(ucpBuffer + i));
        *      ack();
        *  }
        *  read_i2c(*(ucpBuffer + bufferLength - 1));
        *  nack();
        *  stop();
        *
        *  // If no error happened return 0, else return error code.
        *  return (0);
        */

	/*
    Dword                   error = BR_ERR_NO_ERROR;        
    Endeavour* pendeavour = (Endeavour *)bridge;
	Dword       i;
    //if (pendeavour->ctrlBus == BUS_I2C)
    //    error = I2c_readControlBus(bridge, i2cAddr, bufferLength, buffer);
    //else if (pendeavour->ctrlBus == BUS_USB)
    //    error = Usb2_readControlBus(bridge, bufferLength, buffer);

	if (pendeavour->ctrlBus == BUS_I2C)
	{
		 for (i = 0; i < IT9300User_RETRY_I2C_MAX_LIMIT; i++) 
		 {
			error =I2c_readControlBus(bridge, i2cAddr, bufferLength, buffer);
			if (error == 0) return error;
			BrUser_delay (bridge, 1);
		}
	}
	else if (pendeavour->ctrlBus == BUS_USB)
	{
		 for (i = 0; i < IT9300User_RETRY_USB_MAX_LIMIT; i++) 
		 {
			error = Usb2_readControlBus(bridge, bufferLength, buffer);
			if (error == 0) return error;
			BrUser_delay (bridge, 1);
		}
	}
    return (error);*/
	return busRx(bufferLength, buffer);
}

//#include <sys/time.h>
#include <time.h>
Dword BrUser_time (
 	OUT Dword*			time
 ) {
 	//*time = GetTickCount();
 	//return (BR_ERR_NO_ERROR);

 	/*
 	struct timeval tv;
 	if( gettimeofday(&tv, NULL) != 0 )
 		return 0;
 	printf("tv_sec = %d, tv_usec= %d", tv.tv_sec, tv.tv_usec);
 	Dword ret = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
 	printf("ret = %d", ret);
 	return ret;*/


 	struct timespec ts;
 	clock_gettime(CLOCK_MONOTONIC, &ts);
 	//printf("tv_sec = %d, tv_usec= %d", ts.tv_sec, ts.tv_nsec);
 	*time = (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
 	//printf("*time = %d", *time);
 	return 0;
 }

 Dword BrUser_time (
 ) {
 	struct timespec ts;
 	clock_gettime(CLOCK_MONOTONIC, &ts);
 	//printf("tv_sec = %d, tv_usec= %d", ts.tv_sec, ts.tv_nsec);
 	return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
 	//printf("*time = %d", *time);
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

