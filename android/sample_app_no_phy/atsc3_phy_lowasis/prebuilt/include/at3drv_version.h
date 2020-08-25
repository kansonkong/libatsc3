/*
	at3drv_version.h

	for Project Atlas

	Copyright © 2017, Digital STREAM Labs, Inc.
	Copyright © 2017, 2018, 2019 LowaSIS, Inc.
*/

#ifndef __AT3DRV_VERSION_H__
#define __AT3DRV_VERSION_H__

//============================================================================


#define AT3DRV_VER_MJ 2
#define AT3DRV_VER_MN 26
#define AT3DRV_VER_PT 0

// PT (patch) is for hot-patch, usually zero.

/*
	version history (newest version first, old one last)

		2.26.0, 190826
			fx3
				both fw phase-1 and phase-2 are included in at3drv
				phase 1 fw ver: 3.00
					based on fw 2.60 (which is almost similar to fw 2.48)
				phase 2 fw ver: 4.00
					continuation of branch tip
				support nvram blob data loading
			at3drv
				support duplicated dbg module name
				fix issue when logcat output does not output in some situation
				app can change dbglog target eg, stdout or stderr instead of logcat.

		2.25.0, 190821
			internal test release

		2.24.0, 190806
			fx3
				support vsb in delayed fx3 start mode
				fx3 fw 2.72

		2.23.1, 190801
			driver
				libusb-atlas prepie build added

		2.23, 190731
			driver
				AT3_DEV_KEY composition method changed (only in android)
				libusb bugfix
			(Note) there is known issue when using multi usb dongle in android.

		2.22.1, 190729
			driver
				populate api bugfix (only in android)

		2.22, 190726
			api
				finalize at3drv api for android (supporting android p)

		2.21, 190716
			driver, fx3
				use delayed fx3 start, regardless of fe vendor

		2.20, 190712
			driver
				add bootstrap parameter query api for sony fe
			fx3
				fw rolled back to 2.5x (== 2.70)

		2.19.4, 190709 (internal test)
			driver
				support control only session
		2.19.3, 190708 (internal test)
			fx3
				fw rolled back to 2.48
				+fx3 night mode, +ep2watchdog, +vcob_msg command
			driver
				make compatible to fx fw 2.48+

		2.19.2, 190708
		2.19.1, 190705, for internal test
		2.19, 190702, for internal test
			driver
				support fx-night-mode option
			fx3 updated to 2.52
				vcob in ep buffer doubled (2->4)
				disable fx3 arm core data cache
				use REUSE_DMA_CHANNEL config
				dpc mode used for usb setup callback

		2.18, 190620
			api
				eAT3_FESTAT_HW_INFO added for AT3DRV_FE_GetStatus()
			driver
				turn off unsed led for b17+ hw (saving power consumption)
				reset fx3 in case fx3 fw version is older than expected 
				if user specifed wrong fw version, retry open with 'default' demod fw
			build system
				always run fwstore generation in msvc project

		2.17, 190618
			it is for internal test
			driver
				lgsic demod updated
					support b17+ new silicon
					keep supporting b17 old hw also
				lg demod api 3.3, b17+ fw: 00.06.00, b17 fw: 3.1.3
				tune mode changed (autoscan -> normal) for b17 ic
				add new api: query l1 basic & preamble (only for sony fe)
			fx3
				fw version updated to 2.49
				add fw's signal monitor with self led control (b17+)
				improve main event loop, optimize response time
			build system
				msvc: updated to visual studio 2019 (platform tool v142)

		2.16, 190514
			it is for internal test for android P
			driver
				android: usb enumeration is changed. use ETA mode
				  (enumeration thru application)
				dbgutil improved (support early print)
				some at3base header supports header-only build
				
		2.15.3, 190502
			build
				improve windows/linux build environment
				script for auto packing

		2.15.2, 190329
			build
				libat3aflqs merged into libat3afl
				release component is still remain as 'aflqs'
				linux sdk renamed to at3drv_linux_x.y
			fx3
				retry alpu auth test when auth fail at boot time

		2.15, 190311
			api
				new api added: AT3DRV_FE_Control()
			driver
				bugfix in memory alloc/free
				support negative debug module level (suppress-all)
			fx3
				signal monitor in fx3, support lock led (sony fe only)
				fx3 fw updated to 2.47
			build
				automatic packaging/sanity test script (windows)
			mtcp
				updated to 1.3.5

		2.14.1 190220
			driver
				automatic fe vendor probing
			build
				win/msvc: add static libraries, remove x86(win32) support
				improved automated build

		2.14, 190213
			api
				new status query api: eAT3_FESTAT_RF_DETAIL, S_FE_DETAIL
				support query of ea wakeup bits
			driver
				add support sony demod (ashley)
				usb: improve stability in case of bad cable
			fx3
				fix vcob timeout issue
				fx3 fw updated to 2.46
			build
				support latest android studio/ndk

		2.13, 190103
			temporary version for internal test

		2.12, 181228 (tested in windows only)
			at3drv/base
				add mode of pure bbp packet output (instead of bbp container)
				extend tc2 example to support ipfwd (windows only)
				fx3fw updated to 2.45 (use g7-opt2)
				supoprt alp-standard auto detection
			api
				one more AT3RESULT type added
			internal
				add feature of bbpctr file replay
				prepare for fe multi-verdor 
				debug print performance slightly improved
			wip (beta or partial support)
				support sony demod (for non-windows)
				support alp rohc decoding (for non-windows)
			etc
				at3base/socket lib updated (not directly used in at3drv)
				build system modified (not directly used in at3drv)
				code refactoring

		2.11, 181018
			build infra
				linux sdk is now packaged to tar.xz
			at3drv improvement
				use per-device mutex in addition to api mutex
			afl port
				porting afl quick autoscan library

		2.10, 180903
			build system
				support arm-starfish-linux target

		2.9, 180817
			dbgutil improvement, alp-usb mode modify
			changeset : 581600b8b383
		2.8, 180810
			changeset : 82654e223f2f
		2.7, 180716
			minor changes (at3drv.mk, mtcp server ..)
		2.6, 180703
			for client M, RELEASE Library
			develop witi+uranus 

		2.5, 180702
			at3drv
				add cross-build of mipsel-openwrt-linux
				improve vcob exception handling
			fx3 fw updated to 2.44
			upper layer
				uda api layer added 

		2.4, 180615
			at3drv
				user can change ringbuffer size
				user can change ringbuffer wakeup level (atsc3/ts)
				tc2 example multi-thread option
				improve api lock wait in RELEASE build
			layer2
				use L2CFG_NEW_OPTIMIZED_BUFFERING mode
					use rb_wakeup_level tweak
				Wait api added in layer2

		2.3.1, 180611
			bbp DECODABLE_MIN_SIZE changed to 128

		2.3, 180601
			fx3 fw version updated to 2.43
				support usb 3.0 super speed
				support atlas board r0.4
				support bootloader
				support serial number
				manufacturer/product string changed
			bug fix
				cannot support plp id > 3
				device detection fail in windows platform (64 bit devkey)
				unnecessary cpu wakeup in windows
				cannot detect more than 4 devices
				some garbage data after FE_Stop() call
			improvement
				increase ring buffer for stable usb transfer
			minor change
				LED blinking pattern of idle time changed

		2.2, 180313
			support mode that output LMT with parsed lmt pointer
			minor change in android debug log print

		2.1, 180312
			support output mode of IP/LMT
			improve SetPLP api against abnormally used argument
			fix bug in ParseLmt
			add experimental implementation of A/330-2018 ALP
			add option api by which demod firmware can be selected

		2.0, 180220
			r850 tuner clock output disable
			at3drv support aarch64 linux toolchain

		1.9, 180202
			fx3 fw version updated to 2.15 (improve stability)

		1.8, 180201
			at3drv api
				version scheme changed (add patch revision)
				some debug api added (for internal use)
			improvements
				fx3 fw improved to 2.14
				some tc2 example feature added
				some improvement in usb device connection issue
			internal
				at3base module is separated from at3drv.
				some changes for portability

		1.7, 171208
			at3drv api
				fix dprint log is not shown at android.
				implement IP packet output.
					only single packet and single header in ALP.
					need to be implemented about another case.

		1.6, 171205
			at3drv api
				add eAT3_RFSTAT_LGD_PLP_V1
				fx2/fx3 build system improved.
					use prebuilt firmware. useful for windows build.
				fix bug (fx3 fw)
					panic issue when gpif restart at no signal environment.
				fw version:  fx3: 2.13, fx2: 1.04
				now, fx2 is supported by at3drv loader api.
				improve at3drv release script.
				synch'ed to atlas-android repo.  

		1.5, 171120
			at3drv api
				extension name of c++-only-headers changed to hpp.
				AT3DRV_FE_GetStatus() pValue type changed to (void *).
				support more E_AT3_RFSTAT type
					LOCK, STRENGTH, LGDEMOD_V1, PLP.
				AT3DRV_ResetDevice() implemented (for 3 reset type).
				at3drv examples (ex-rxdata) improved
			layer2
				all apis implemented (including CAt3DrvLayer2::GetPLPInfo, ..)
				C api added. use at3drvl2_api.h header for C api.
				example ex-layer2 now use C api.

		1.4, 171116
			at3drv
				AT3DRV_ResetDevice() 2nd argument added
				FE_GetStatus(), eAT3_RFSTAT_LGDEMOD_V1 added, eAT3_RFSTAT_LOCK changed.
				fixed bug: FE_Start() plp id not working
			at3drv layer2 added, support yocto poky toolchain
		
		1.3, 171026
			at3drv
				improvement
					Tuner(r850) tuning result applied.

		1.2, 170922 (f1298afedf2e)
			at3drv
				API changes:
					AT3DRV_CancelWait() api added.
					AT3RES_CANCEL enum type added. eAT3_RXDTYPE_ALP is now supported.
					Repeated FE_Start (without FE_Stop) is allowed.
					AT3DRV_FE_GetStatus is partially implemented.
					All API call (except Option apis) should be called after AT3DRV_Init().
				important changes:
					BBP container packet now has correct payload data.
					BBP packet with error indicator is dropped (not passed to user).
					USB key (unique id) generation format is changed.
				trivial changes
					improve portablilty. (linux, android, windws)
					API mutex does not support recusive lock any more.
						however, AT3DRV_WaitRxData call can be overlapped.
					ring buffer improved (stability, external cancelation support)
					libusb context is shared between core and loader apis.
					AT3_DelayMs misc api added.
				example chagnes:
					ex-rxdata example added
			buildsystem
				use customized libusb as default (included in repo)

		1.1, 170831
			at3drv
				API changes:
					AT3DRV_Init() dropped option parameter.
					AT3DRV_LDR_CheckDeviceExist() renamed.
					AT3DRV_GetVersionInfo() added.
				support AT3DRV_OpenDevice() key parameter.
				support mpeg2 ts reception. 
				support eAT3_RXDTYPE_BBPCTR and eAT3_RXDTYPE_TS.
				dropped usbid and usbport options.
			tc2, examples
				improve examples to load firmware automatically.
				gcc extension features removed in tc2 app.
				devkey, listkeys option added in tc2.
				support shared lib build option.

		1.0, 170823
			initial public release
*/

//============================================================================

#define __stringify(a) _stringify(a)
#define _stringify(a) #a

// do not modify manually below macro.

#define AT3DRV_VER ((AT3DRV_VER_MJ << 16) | (AT3DRV_VER_MN << 8) | (AT3DRV_VER_PT))

#if AT3DRV_VER_PT != 0
  #define AT3DRV_VER_STR __stringify(AT3DRV_VER_MJ) "." __stringify(AT3DRV_VER_MN) "." __stringify(AT3DRV_VER_PT)
#else
  #define AT3DRV_VER_STR __stringify(AT3DRV_VER_MJ) "." __stringify(AT3DRV_VER_MN)
#endif


#endif // __AT3DRV_VERSION_H__

