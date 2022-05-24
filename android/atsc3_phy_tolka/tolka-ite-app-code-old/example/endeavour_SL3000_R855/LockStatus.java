package com.example.endeavour_SL3000_R855;

import com.api.Debug;
import com.api.Error;

import android.icu.math.BigDecimal;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.support.v4.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.RadioButton;
import android.widget.RadioGroup;

public class LockStatus extends Fragment
{
    private static final String TAG = "Endeavour.LockStatus";
    private static Thread thread;
    private volatile boolean terminate = false;

    private static ListView listViewStatusinfo;

    private static StatusInfoAdapter statusAdapter;

    private static String[] Title;
    private static String[] Content;

    private static String[] Title_ATSC3      = {"TS Lock State",       "Unlock Detected", "SNR", "RSSI", "Confidence", "PLP 0", "PLP 1", "PLP 2", "PLP 3"};
    private static String[] Content_ATSC3    = {"0",                   "0",               "0",   "0",    "0",          "0",     "0",     "0",     "0"};

    private static String[] Title_ATSC1      = {"TS Lock State",       "Unlock Detected", "SNR", "BER", "PER", "RSSI", "Confidence"};
    private static String[] Content_ATSC1    = {"0",                   "0",               "0",   "0",   "0",   "0",    "0"};

    private static String[] Title_DVB_T_T2   = {"Sync State",     "TS Lock State",  "Unlock Detected",                        "SNR", "BER", "PEN", "RF Level dB", "SQI", "SSI"};
    private static String[] Content_DVB_T_T2 = {"0",              "0",              "0",                                      "0",   "0",   "0",   "0",           "0",   "0"};

    private static String[] Title_ISDBT      = {"Dmd Lock",       "TS Lock",        "Unlock Detected",                        "SNR", "BERA","BERB","BERC", "PENA","PENB", "PENC", "RF Level dB"};
    private static String[] Content_ISDBT    = {"0",              "0",              "0",                                      "0",   "0",   "0",   "0",    "0",   "0",    "0",    "0"};

    private static String[] Title_J83B       = {"Ar Lock",        "TS Lock",        "Unlock Detected",                        "SNR", "BER", "PEN", "RF Level dB"};
    private static String[] Content_J83B     = {"0",              "0",              "0",                                      "0",   "0",   "0",   "0"};

    private static String[] Title_DVB_C      = {"Sync State",     "TS Lock State",  "Unlock Detected",                        "SNR", "BER", "PEN", "RF Level dB"};
    private static String[] Content_DVB_C    = {"0",              "0",              "0",                                      "0",   "0",   "0",   "0"};


    private static DeviceSet.MODE mode = DeviceSet.MODE.ATSC1;
    private static int deviceId = 0;
    
    private static final int STATUS_UPDATE = 0;
    private static final int STATUS_CLEAR = 1;
    
    public Handler handle = new Handler()
    {
        @RequiresApi(api = Build.VERSION_CODES.N)
        public void handleMessage(Message msg)
        {
            switch(msg.what)
            {                
                case STATUS_UPDATE:
                {
                    long error = 0;

                    if(mode == DeviceSet.MODE.ATSC3) {
                        int[]       confidence = {0};
                        boolean[]   tsLockState = {false};
                        boolean[]   unlockDetected = {false};
                        double[]    snr = {0};
                        int[]       plpValid = {0, 0, 0, 0};
                        int[]       rssi = {0};
                        BigDecimal  bigDecimal;

                        error = MainActivity.endeavour.TunerDemod_Atsc3GetStatus(deviceId, confidence, tsLockState, unlockDetected, snr, plpValid, rssi);

                        if (error > 0) {
                            Log.v(TAG, "SL3000_R855_Atsc3GetStatus error = 0x" + Long.toHexString(error));
                        }
                        if (error == Error.Error_NO_ERROR) {
                            // status
                            Content_ATSC3[0] = Boolean.toString(tsLockState[0]);
                            Content_ATSC3[1] = Boolean.toString(unlockDetected[0]);
                            bigDecimal = new BigDecimal(String.format("%3.2f", snr[0]));
                            Content_ATSC3[2] = bigDecimal.toString() + " dB";
                            Content_ATSC3[3] = Integer.toString(rssi[0]) + " dBm";
                            Content_ATSC3[4] = Integer.toString(confidence[0]) + " %";
                            if(plpValid[0] == 0)    Content_ATSC3[5] =  "Unlock";
                            else                    Content_ATSC3[5] =  "Lock";
                            if(plpValid[1] == 0)    Content_ATSC3[6] =  "Unlock";
                            else                    Content_ATSC3[6] =  "Lock";
                            if(plpValid[2] == 0)    Content_ATSC3[7] =  "Unlock";
                            else                    Content_ATSC3[7] =  "Lock";
                            if(plpValid[3] == 0)    Content_ATSC3[8] =  "Unlock";
                            else                    Content_ATSC3[8] =  "Lock";
                            statusAdapter.clear();
                            statusAdapter.add(new StatusInfo("Status Infomation", "", true));
                            for (int i = 0; i < Title_ATSC3.length; i++)
                                statusAdapter.add(new StatusInfo(Title_ATSC3[i], Content_ATSC3[i], false));
                            statusAdapter.notifyDataSetChanged();
                            statusAdapter.setNotifyOnChange(true);
                        }
                    } else if (mode == DeviceSet.MODE.ATSC1) {
                        int[]       confidence = {0};
                        boolean[]   tsLockState = {false};
                        boolean[]   unlockDetected = {false};
                        int[]       snr = {0};
                        double[]    ber = {0};
                        double[]    per = {0};
                        int[]       rssi = {0};
                        BigDecimal  bigDecimal;

                        error = MainActivity.endeavour.TunerDemod_Atsc1GetStatus(deviceId, confidence, tsLockState, unlockDetected, snr, ber, per, rssi);

                        if (error > 0) {
                            Log.v(TAG, "SL3000_R855_Atsc1GetStatus error = 0x" + Long.toHexString(error));
                        }

                        if (error == Error.Error_NO_ERROR) {
                            // status
                            Content_ATSC1[0] = Boolean.toString(tsLockState[0]);
                            Content_ATSC1[1] = Boolean.toString(unlockDetected[0]);
                            Content_ATSC1[2] = Integer.toString(snr[0]) + " dB";
                            bigDecimal = new BigDecimal(String.format("%1.2e", ber[0]));
                            Content_ATSC1[3] = bigDecimal.toString();
                            bigDecimal = new BigDecimal(String.format("%1.2e", per[0]));
                            Content_ATSC1[4] = bigDecimal.toString();
                            Content_ATSC1[5] = Integer.toString(rssi[0]) + " dBm";
                            Content_ATSC1[6] = Integer.toString(confidence[0]) + " %";
                            statusAdapter.clear();
                            statusAdapter.add(new StatusInfo("Status Infomation", "", true));
                            for (int i = 0; i < Title_ATSC1.length; i++)
                                statusAdapter.add(new StatusInfo(Title_ATSC1[i], Content_ATSC1[i], false));
                            statusAdapter.notifyDataSetChanged();
                            statusAdapter.setNotifyOnChange(true);
                        }
                    }
                    else if (mode == DeviceSet.MODE.DVB_T_T2) {
                        Log.e(TAG, "Unsupported DVB_T_T2 Mode!");
                    }
                    else if (mode == DeviceSet.MODE.ISDBT) {
                        Log.e(TAG, "Unsupported ISDBT Mode!");
                    }
                    else if (mode == DeviceSet.MODE.J83B) {
                        Log.e(TAG, "Unsupported J83B Mode!");
                    }
                    else if (mode == DeviceSet.MODE.DVBC) {
                        Log.e(TAG, "Unsupported DVBC Mode!");
                    }
                    else {
                        Log.e(TAG, "Unsupported TV Mode!");
                    }
                }
                break;
                
                case STATUS_CLEAR:
                {
                    if (mode == DeviceSet.MODE.ATSC3) {
                        // status
                        Content_ATSC3[0] = "0";
                        Content_ATSC3[1] = "0";
                        Content_ATSC3[2] = "0";
                        Content_ATSC3[3] = "0";
                        Content_ATSC3[4] = "0";
                        Content_ATSC3[5] = "0";
                        Content_ATSC3[6] = "0";
                        Content_ATSC3[7] = "0";
                        Content_ATSC3[8] = "0";

                        statusAdapter.clear();
                        statusAdapter.add(new StatusInfo("Status Infomation", "", true));
                        for (int i = 0; i < Title_ATSC3.length; i++)
                            statusAdapter.add(new StatusInfo(Title_ATSC3[i], Content_ATSC3[i], false));
                        statusAdapter.notifyDataSetChanged();
                        statusAdapter.setNotifyOnChange(true);
                    } else if (mode == DeviceSet.MODE.ATSC1){
                        // status
                        Content_ATSC1[0] = "0";
                        Content_ATSC1[1] = "0";
                        Content_ATSC1[2] = "0";
                        Content_ATSC1[3] = "0";
                        Content_ATSC1[4] = "0";
                        Content_ATSC1[5] = "0";
                        Content_ATSC1[6] = "0";

                        statusAdapter.clear();
                        statusAdapter.add(new StatusInfo("Status Infomation", "", true));
                        for (int i = 0; i < Title_ATSC1.length; i++)
                            statusAdapter.add(new StatusInfo(Title_ATSC1[i], Content_ATSC1[i], false));
                        statusAdapter.notifyDataSetChanged();
                        statusAdapter.setNotifyOnChange(true);
                    }
                    else if (mode == DeviceSet.MODE.DVB_T_T2){
                        Log.e(TAG, "Unsupported DVB_T_T2 Mode!");
                    }
                    else if (mode == DeviceSet.MODE.ISDBT){
                        Log.e(TAG, "Unsupported ISDBT Mode!");
                    }
                    else if (mode == DeviceSet.MODE.J83B){
                        Log.e(TAG, "Unsupported J83B Mode!");
                    }
                    else if (mode == DeviceSet.MODE.DVBC){
                        Log.e(TAG, "Unsupported DVBC Mode!");
                    }
                    else {
                        Log.e(TAG, "Unsupported TV Mode!");
                    }
                }
                break;              
            }
        }
    };
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
    
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInsatnceState)
    {
        if(container == null)
        {
            return null;
        }
        else
        {
            View view = inflater.inflate(R.layout.fragment_lockstatus, container, false);

            Title   = Title_ATSC1;
            Content = Content_ATSC1;

            listViewStatusinfo = (ListView) view.findViewById(R.id.lockstatus_statusinfo);
            statusAdapter = new StatusInfoAdapter(listViewStatusinfo.getContext());
            statusAdapter.add(new StatusInfo("Status Information", "", true));
            for(int i = 0; i < Title.length; i++)
                statusAdapter.add(new StatusInfo(Title[i], Content[i], false));
            //for(int i = 0; i < Title_ATSC3.length; i++)
            //    statusAdapter.add(new StatusInfo(Title_ATSC3[i], Content_ATSC3[i], false));
            listViewStatusinfo.setAdapter(statusAdapter);

            // standard
            RadioGroup standardGroup = (RadioGroup) view.findViewById(R.id.lockstatus_standard_group);
            standardGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {

                @Override
                public void onCheckedChanged(RadioGroup group, int checkedId) {
                    // TODO Auto-generated method stub

                    Message messageClear = new Message();
                    messageClear.what = STATUS_CLEAR;
                    handle.sendMessage(messageClear);

                    switch(checkedId)
                    {
                        case R.id.lockstatus_atsc3:
                            mode = DeviceSet.MODE.ATSC3;
                            break;
                        case R.id.lockstatus_atsc1:
                            mode = DeviceSet.MODE.ATSC1;
                            break;
                        case R.id.lockstatus_dvb_t_t2:
                            mode = DeviceSet.MODE.DVB_T_T2;
                            break;
                        case R.id.lockstatus_isdbt:
                            mode = DeviceSet.MODE.ISDBT;
                            break;
                        case R.id.lockstatus_j83b:
                            mode = DeviceSet.MODE.J83B;
                            break;
                        case R.id.lockstatus_dvb_c:
                            mode = DeviceSet.MODE.DVBC;
                            break;
                    }

                    if(DeviceSet.isSetChannel[mode.ordinal()][deviceId])
                    {
                        Log.v(TAG, Debug.getLineInfo() + ", mode = " + mode + "deivceId = " + deviceId + ", " + DeviceSet.isSetChannel[mode.ordinal()][deviceId]);
                        Message messageUpdate = new Message();
                        messageUpdate.what = STATUS_UPDATE;
                        handle.sendMessage(messageUpdate);
                    }

                }
            });

            RadioButton atsc3    = (RadioButton) view.findViewById(R.id.lockstatus_atsc3);
            RadioButton atsc1    = (RadioButton) view.findViewById(R.id.lockstatus_atsc1);
            RadioButton dvb_t_t2 = (RadioButton) view.findViewById(R.id.lockstatus_dvb_t_t2);
            RadioButton isdbt    = (RadioButton) view.findViewById(R.id.lockstatus_isdbt);
            RadioButton j83b     = (RadioButton) view.findViewById(R.id.lockstatus_j83b);
            RadioButton dvbc     = (RadioButton) view.findViewById(R.id.lockstatus_dvb_c);

            atsc3.setEnabled(true);
            atsc1.setEnabled(true);
            dvb_t_t2.setEnabled(false);     // no used for R855
            isdbt.setEnabled(false);        // no used for R855
            j83b.setEnabled(false);         // no used for R855
            dvbc.setEnabled(false);         // no used for R855

            // defalut
            atsc1.setChecked(true);

            // deivce
            final RadioGroup deviceGroup = (RadioGroup) view.findViewById(R.id.lockstatus_device_group);
            deviceGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {

                @Override
                public void onCheckedChanged(RadioGroup group, int checkedId) {
                    // TODO Auto-generated method stub

                    Message messageClear = new Message();
                    messageClear.what = STATUS_CLEAR;
                    handle.sendMessage(messageClear);

                    switch(checkedId)
                    {
                        case R.id.lockstatus_device_0:
                            deviceId = 0;
                            break;
                        case R.id.lockstatus_device_1:
                            deviceId = 1;
                            break;
                        case R.id.lockstatus_device_2:
                            deviceId = 2;
                            break;
                        case R.id.lockstatus_device_3:
                            deviceId = 3;
                            break;
                    }

                    if(DeviceSet.isSetChannel[mode.ordinal()][deviceId])
                    {
                        Log.v(TAG, Debug.getLineInfo() + ", mode = " + mode + ", deivceId = " + deviceId + ", " + DeviceSet.isSetChannel[mode.ordinal()][deviceId]);
                        Message messageUpdate = new Message();
                        messageUpdate.what = STATUS_UPDATE;
                        handle.sendMessage(messageUpdate);
                    }

                }
            });

            RadioButton deviceId0 = (RadioButton) view.findViewById(R.id.lockstatus_device_0);
            RadioButton deviceId1 = (RadioButton) view.findViewById(R.id.lockstatus_device_1);
            RadioButton deviceId2 = (RadioButton) view.findViewById(R.id.lockstatus_device_2);
            RadioButton deviceId3 = (RadioButton) view.findViewById(R.id.lockstatus_device_3);

            // defalut
            deviceId0.setChecked(true);

            if(MainActivity.channels == 1)
			{
				deviceId1.setEnabled(false);
				deviceId2.setEnabled(false);
				deviceId3.setEnabled(false);
			}

            terminate = false;

            thread = new Thread(new Runnable()
            {
                public void run()
                {
                    try
                    {
                        while(!terminate)
                        {
                            if(DeviceSet.isSetChannel[mode.ordinal()][deviceId])
                            {
                                Log.v(TAG, Debug.getLineInfo() + ", " + DeviceSet.isSetChannel[mode.ordinal()][deviceId]);
                                Message message = new Message();
                                message.what = STATUS_UPDATE;
                                handle.sendMessage(message);
                            }
                            Thread.sleep(1000);
                        }
                    }
                    catch(InterruptedException e)
                    {
                        e.printStackTrace();
                    }
                }
            });
            thread.start();
            
            return view;
        }
    }
    
    @Override
    public void onStop()
    {
        terminate = true;
        super.onStop();
    }
    
    private class LockInfo
    {
        String title;
        int resid;
        
        public LockInfo(String t, int i)
        {
            title = t;
            resid = i;
        }
    }
     
    private class LockInfoAdapter extends ArrayAdapter<LockInfo> {
        LayoutInflater inflator;
 
        public LockInfoAdapter(Context context) {
            super(context, 0);
            inflator = LayoutInflater.from(context);
        }
 
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = inflator.inflate(
                        R.layout.listconent, parent, false);
            }
 
            TextView text = (TextView) convertView
                    .findViewById(R.id.listcontent_title);
            ImageView image = (ImageView) convertView
                    .findViewById(R.id.listcontent_logo);

            LockInfo s = this.getItem(position);
 
            text.setText(s.title);
            image.setImageResource(s.resid);
 
            return convertView;
        }
    }
    
    private class StatusInfo
    {
        String item;
        String content;
        public StatusInfo(String i, String c, boolean h)
        {
            item    = i;
            content = c;
        }
    }
     
    private class StatusInfoAdapter extends ArrayAdapter<StatusInfo> {
        LayoutInflater inflator;
 
        public StatusInfoAdapter(Context context) {
            super(context, 0);
            inflator = LayoutInflater.from(context);
        }
 
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = inflator.inflate(
                        android.R.layout.simple_list_item_2, parent, false);
            }

            StatusInfo s = this.getItem(position);
            
            TextView text1 = (TextView) convertView
                    .findViewById(android.R.id.text1);
            TextView text2 = (TextView) convertView
                    .findViewById(android.R.id.text2);

            {
                text1.setText(s.item);
                text2.setText("" + s.content);              
            }
 
            return convertView;
        }
    }   
}