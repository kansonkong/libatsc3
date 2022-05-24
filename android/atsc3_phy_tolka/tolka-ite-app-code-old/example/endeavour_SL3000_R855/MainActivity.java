package com.example.endeavour_SL3000_R855;


import android.app.PendingIntent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.os.Bundle;
import android.support.design.widget.BottomNavigationView;
import android.support.v7.app.AppCompatActivity;
import android.support.annotation.NonNull;
import android.view.MenuItem;
import android.support.v4.view.ViewPager;
import android.support.v4.view.ViewPager.OnPageChangeListener;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentPagerAdapter;
import android.util.Log;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.hardware.usb.UsbManager;

import com.api.Endeavour;
import com.api.Debug;
import com.api.Error;
import android.widget.Toast;
import java.util.List;
import java.util.ArrayList;
import com.api.User;


public class MainActivity extends AppCompatActivity implements OnPageChangeListener{
    private static String TAG = "Endeavour.Main";
    private ViewPager viewPager;
    private BottomNavigationView navView;

    private UsbManager manager;
    public static Endeavour endeavour;

    public static boolean isOpen = false;
    public static int channels = User.TUNER_CHANNEL;//1, 2, 4 ch tuner;

    private BottomNavigationView.OnNavigationItemSelectedListener mOnNavigationItemSelectedListener
            = new BottomNavigationView.OnNavigationItemSelectedListener() {

        @Override
        public boolean onNavigationItemSelected(@NonNull MenuItem item) {
            switch (item.getItemId()) {
                case R.id.navigation_home:
                    viewPager.setCurrentItem(0);
                    return true;
                case R.id.navigation_dashboard:
                    viewPager.setCurrentItem(1);
                    return true;
                case R.id.navigation_notifications:
                    viewPager.setCurrentItem(2);
                    return true;
            }
            return false;
        }
    };

	private void openUsbDevice(){
        //before open usb device
        //should try to get usb permission
        tryGetUsbPermission();
    }
	
	private static final String ACTION_USB_PERMISSION = "com.android.example.USB_PERMISSION";

	private boolean isWeCaredUsbDevice(UsbDevice usbDevice) {
		return endeavour.checkDevice(usbDevice, this, R.xml.device_filter);
	}

	private void tryGetUsbPermission(){
        manager = (UsbManager) getSystemService(Context.USB_SERVICE);
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        PendingIntent mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
        //here do emulation to ask all connected and cared usb device for permission
        for (final UsbDevice usbDevice : manager.getDeviceList().values()) {
            //check VID and PID
            if(isWeCaredUsbDevice(usbDevice)){
				Log.d(TAG, usbDevice.getDeviceName());
	            if(manager.hasPermission(usbDevice)){
	                //if has already got permission, just goto connect it
	                //that means: user has choose yes for your previously popup window asking for grant perssion for this usb device
	                //and also choose option: not ask again
	                afterGetUsbPermission(usbDevice);
	            }else{
	                //this line will let android popup window, ask user whether to allow this app to have permission to operate this usb device
	                manager.requestPermission(usbDevice, mPermissionIntent);
	            }   
            }
        }
    }
    private void afterGetUsbPermission(UsbDevice usbDevice){
        //call method to set up device communication
        //Toast.makeText(this, String.valueOf("Got permission for usb device: " + usbDevice), Toast.LENGTH_LONG).show();
        //Toast.makeText(this, String.valueOf("Found USB device: VID=" + usbDevice.getVendorId() + " PID=" + usbDevice.getProductId()), Toast.LENGTH_LONG).show();
        doYourOpenUsbDevice(usbDevice);
    }
 
    private void doYourOpenUsbDevice(UsbDevice usbDevice){
		long error = endeavour.openDevice(manager, usbDevice);
		if (error == Error.Error_NO_ERROR)
			isOpen = true;
		else
			Debug.showmessage(this, "failed to open USB device!", error);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        navView = findViewById(R.id.nav_view);
        navView.setOnNavigationItemSelectedListener(mOnNavigationItemSelectedListener);

        viewPager = (ViewPager)findViewById(R.id.viewPager);
        viewPager.setAdapter(new HomeViewPagerAdapter(getSupportFragmentManager()));
        viewPager.addOnPageChangeListener(this);

        // check device is attach
        Intent intent = getIntent();
        if(intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)) {
            Log.v(TAG, "device attahced");
            isOpen = false;
        }

        if(!isOpen) {

            // create device instance
            manager = (UsbManager)getSystemService(Context.USB_SERVICE);

            // create Endeavour
            endeavour = new Endeavour();

			// used to check permission first and then open device if the permission is granted
			openUsbDevice();

			// old code: open device without checking USB permission
			/*
            		long error = endeavour.open(manager, this, R.xml.device_filter);
            		if(error == Error.Error_USB_DEVICE_NOT_FOUND)
                		Debug.showmessage(this, "Device isn't be found", error);
            		else if(error == Error.Error_USB_PID_VID_WRONG)
                		Debug.showmessage(this, "Either PID or VID is wrong", error);
            if(error == Error.Error_NO_ERROR)
                isOpen = true;
			*/
        }

		// listen for device's action
        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_DEVICE_DETACHED);
		filter.addAction(ACTION_USB_PERMISSION);
        registerReceiver(usbReceiver, filter);        
    }

    public void exit()
    {
        Log.v(TAG, Debug.getLineInfo());

        for(int j = 0; j < 2; j++) {
            for (int i = 0; i < 4; i++)
                DeviceSet.isSetChannel[j][i] = false;
        }

        endeavour.IT9300_reboot(0);// will turn on app again, close it

        //unregisterReceiver(mUsbReceiver);// app close, useless
        finish();
        System.exit(0);
    }

    private final BroadcastReceiver usbReceiver = new BroadcastReceiver() {

        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            if (UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action)) {
                Log.v(TAG, "ACTION_USB_DEVICE_DETACHED");
                exit();
            }

			if (ACTION_USB_PERMISSION.equals(action)) {
				Log.v(TAG, "ACTION_USB_PERMISSION");
                synchronized (this) {
                    UsbDevice usbDevice = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        //user choose YES for your previously popup window asking for grant perssion for this usb device
                        if(null != usbDevice){
							Log.v(TAG, usbDevice.getDeviceName());
                            afterGetUsbPermission(usbDevice);
                        }
                    }
                    else {
                        //user choose NO for your previously popup window asking for grant perssion for this usb device
                        Toast.makeText(context, String.valueOf("Permission denied for device" + usbDevice), Toast.LENGTH_LONG).show();
                    }
                }
            }
        }
    };

    public class HomeViewPagerAdapter extends FragmentPagerAdapter {

        public HomeViewPagerAdapter(android.support.v4.app.FragmentManager fm) {
            super(fm);
        }

        @Override
        public Fragment getItem(int iPosition) {
            Log.v(TAG, "iPosition = " + iPosition);

            switch (iPosition) {
                case 0:
                    return new DeviceSet();
                case 1:
                    return new LockStatus();
                case 2:
                    return new TsAnalysis();
            }
            return null;
        }

        @Override
        public int getCount() {
            return 3;

        }
    }

    @Override
    public void onPageScrollStateChanged(int arg0) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onPageScrolled(int arg0, float arg1, int arg2) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onPageSelected(int arg0) {
        // TODO Auto-generated method stub
        navView.getMenu().getItem(arg0).setChecked(true);
    }

}
