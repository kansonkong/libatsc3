package com.api;

import android.content.Context;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbDevice;
import android.util.Log;

public class Endeavour {
	public static final String TAG = "Endeavour";
	
	public long open(UsbManager manager, Context context, int device_filter) {
		return Bus.initial(manager, context, device_filter);
	}

	public boolean checkDevice(UsbDevice device, Context context, int device_filter) {
		return Bus.checkDevice(device, context, device_filter);
	}

	public long openDevice(UsbManager manager, UsbDevice device) {
		return Bus.openDevice(manager, device);
	}

	public native long init(int tvStandard);

	// SL3000_R855
	public native long TunerDemod_Atsc3Tune(
			int		id,
			int		frequencyKHz,
			int		bandwidthKHz);

	public native long TunerDemod_Atsc3SetPLP(
			int		id,
			char	plpMask,
			char[] 	plpID);

	public native long TunerDemod_Atsc3GetStatus(
			int 		id,
			int[]		confidence,
			boolean[]	tsLockState,
			boolean[]	unlockDetected,
			double[]	snr,
			int[]		plpValid,
			int[]		rssi);

	public native long TunerDemod_Atsc1Tune(
			int		id,
			int		frequencyKHz);

	public native long TunerDemod_Atsc1GetStatus(
			int 		id,
			int[]		confidence,
			boolean[]	tsLockState,
			boolean[]	unlockDetected,
			int[]		snr,
			double[]	ber,
			double[]	per,
			int[]		rssi);

	public native long Demod_Atsc1WriteRegisters(
			int 		id,
			int        registerAddress,
			int        bufferLength,
			byte[]	   buffer);

	public native long Demod_Atsc1ReadRegisters(
			int 		id,
			int        registerAddress,
			int        bufferLength,
			byte[]	   buffer);


	// IT9300 (Endeavour)
	public native long IT9300_writeRegisters (
			int		chip,
			int		registerAddress,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_writeRegister (
			int		chip,
			int		registerAddress,
			int		value);
	
	public native long IT9300_writeEepromValues (
			int		chip,
			int		registerAddress,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_writeRegisterBits (
			int		chip,
			int		registerAddress,
			int		position,
			int		length,
			int		value);
	
	public native long IT9300_readRegisters (
			int		chip,
			int		registerAddress,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_readRegister (
			int		chip,
			int		registerAddress,
			int[]	buffer);
	
	public native long IT9300_readEepromValues (
			int		chip,
			int		registerAddress,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_readRegisterBits (
			int		chip,
			int		registerAddress,
			int		position,
			int		length,
			int[]	value);
	
	public native long IT9300_setInTsPktLen (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_setSyncByteMode (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_setTagMode (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_setPidRemapMode (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_enableTsPort (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_disableTsPort (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_setInTsType (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_enPidFilterAT (
			int		chip,
			int		tsSrcIdx,
			int		tableindex);
	
	public native long IT9300_disPidFilterAT (
			int		chip,
			int		tsSrcIdx,
			int		tableindex);

	public native long IT9300_enPidFilter (
			int		chip,
			int		tsSrcIdx);
	
	public native long IT9300_setExternalclock (
			int		chip,
			boolean	bvalue);
	
	public native long IT9300_setNullpacket (
			int		chip,
			boolean	bvalue);
	
	public native long IT9300_modigyTEIerror (
			int		chip,
			boolean	bvalue);
	
	public native long IT9300_setIgnoreFail (
			int		chip,
			boolean	bvalue);
	
	// TS_PARALLEL = 0, //TS_SERIAL = 1,
	public native long IT9300_setOutputTsType (
			int		chip,
			int		tsoutType);
	
	public native long IT9300_setOutputclock (
			int		chip,
			int		value);
	
	public native long IT9300_settestmode (
			int		chip,
			int		tsSrcIdx,
			int		mode);
	
	public native long IT9300_setDataRate (
			int		chip,
			int		tsSrcIdx,
			int		value);
	
	public native long IT9300_writeGenericRegisters (
			int		chip,
			int		bus,
			int		slaveAddress,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_readGenericRegisters (
			int		chip,
			int		bus,
			int		slaveAddress,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_getIrCode (
			int		chip,
			int[]	code);
	
	public native long IT9300_reboot (
			int		chip);
	
	//UART_BAUDRATE_9600 = 0, UART_BAUDRATE_19200 = 1, UART_BAUDRATE_38400 = 2
	public native long IT9300_setUARTBaudrate (
			int		chip,
			int		baudrate);
	
	public native long IT9300_sentUARTData (
			int		chip,
			int		bufferLength,
			int[]	buffer);
	
	public native long IT9300_SetSuspend (
			int		chip);
	
	public native long IT9300_SetSuspendback (
			int		chip);
	
	public native long IT9300_readGenericRegistersExtend (
			int		chip,
			int		bus,
			int		slaveAddress,
			int		repeatStartEnable,
		    int		wBufferLength,
		    int[]	wBuffer,
			int		rBufferLength,
			int[]	rBuffer);
			
	public native long IT9300_setTsEncryptKey (
			int		chip,
			int[]	buffer);

	public native long IT9300_setTsEncrypt (
			int		chip,
			boolean	benable);

	public native long IT9300_simplePidFilter_AddPid (
			int		chip,
			int		tsSrcIdx,
			int		pid,
			int		index);

	public native long IT9300_simplePidFilter_RemovePid (
			int		chip,
			int		tsSrcIdx,
			int		index);

	public native long IT9300_simplePidFilter_ResetPidTable (
			int		chip,
			int	tsSrcIdx);

	public native long IT9300_simplePidFilter_DumpPidTable (
			int		chip,
			int		tsSrcIdx,
			int[]	pidMode,
			int[]	pid,
			int[]	enable);

	public native long IT9300_simplePidFilter_SetMode (
			int		chip,
			int		tsSrcIdx,
			int		pidMode); // 0: disable, 1: pass, 2: block

	// callback
	public static long busTx(int bufferLength, byte[] buffer)
	{
		return Bus.Tx(bufferLength, buffer);
	}
	
	public static long busRx(int bufferLength, byte[] buffer)
	{
		return Bus.Rx(bufferLength, buffer);
	}
	
	// test
	public native String testJ2CString();
	public native void testC2JCallback();
    public static int Add(int x,int y)
    {
    	Log.v(TAG, "x = " + x + ",y = " + y + ",x + y = " + (x+y));
        return x+y;
    }
    
	static {
        System.loadLibrary("Endeavour-jni");
	}
}
