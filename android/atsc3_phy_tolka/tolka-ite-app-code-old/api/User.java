package com.api;

public class User {
	public static final int IT9300User_MAX_PKT_SIZE = 255;

	//#define IT9300User_RETRY_MAX_LIMIT            100//10
	public static final int IT9300User_RETRY_USB_MAX_LIMIT = 1;//10
	public static final int IT9300User_RETRY_I2C_MAX_LIMIT = 100;//10

	/** Define I2C master speed, the default value 0x07 means 366KHz (1000000000 / (24.4 * 16 * IT9300User_I2C_SPEED)). */
	public static final int IT9300User_I2C_SPEED = 0x07;


	/** Define USB frame size */
	public static final int IT9300User_USB20_MAX_PACKET_SIZE = 512;
	public static final int IT9300User_USB20_FRAME_SIZE = 188*87; //jjustman-2022-05-19 - changed from: (188 * 87);	/* get frame size */
	public static final int IT9300User_USB20_FRAME_NUNBER = 1;//4;	/* get frame number */
	public static final int IT9300User_USB20_FRAME_SIZE_DW = (IT9300User_USB20_FRAME_SIZE / 4);
	public static final int IT9300User_USB11_MAX_PACKET_SIZE = 64;
	public static final int IT9300User_USB11_FRAME_SIZE = (188 * 21);
	public static final int IT9300User_USB11_FRAME_SIZE_DW = (IT9300User_USB11_FRAME_SIZE / 4);
	
	/* channel configure: 1/2/4 ch Demond tuner*/
	public static final int TUNER_CHANNEL = 1;//2, 4;

}
