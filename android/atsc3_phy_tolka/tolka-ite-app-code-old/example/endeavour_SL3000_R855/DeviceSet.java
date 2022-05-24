package com.example.endeavour_SL3000_R855;

import com.api.Debug;
import com.api.Error;
import com.api.Bus;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.support.v4.app.Fragment;
import android.text.InputType;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

public class DeviceSet extends Fragment {
    private static final String TAG = "Endeavour.DeviceSet";
    
    public static boolean isInitOk = false;
    public static boolean isSetChannel[][] = {{false, false, false, false},
                                              {false, false, false, false},
                                              {false, false, false, false},
                                              {false, false, false, false},
                                              {false, false, false, false},
                                              {false, false, false, false}};

    private static String Title[];
    private static String Content[];

    private static String Title_ATSC3[]      = {"Standard", "Device ID", "Frequency(KHz)", "Bandwidth(kHz)"};
    private static String Content_ATSC3[]    = {"0",    "0",         "0",              "0"};

    private static String Title_ATSC1[]      = {"Standard", "Device ID", "Frequency(KHz)"};
    private static String Content_ATSC1[]    = {"0",    "0",         "0"};

    private static String Title_DVB_T_T2[]   = {"Standard", "Device ID", "Frequency(KHz)", "Bandwidth(kHz)"};
    private static String Content_DVB_T_T2[] = {"0",    "0",         "0",              "0"};

    private static String Title_ISDBT[]      = {"Standard", "Device ID", "Frequency(KHz)"};
    private static String Content_ISDBT[]    = {"0",    "0",         "0"};

    private static String Title_J83B[]       = {"Standard", "Device ID", "Frequency(KHz)"};
    private static String Content_J83B[]     = {"0",    "0",         "0"};

    private static String Title_DVB_C[]   = {"Standard", "Device ID", "Frequency(KHz)", "Bandwidth(kHz)"};
    private static String Content_DVB_C[] = {"0",    "0",         "0",              "0"};

    private static String Mode[] = {MODE.ATSC3.toString(),
                                    MODE.ATSC1.toString(),
/*                                    MODE.DVB_T_T2.toString(),
                                    MODE.ISDBT.toString(),
                                    MODE.J83B.toString(),
                                    MODE.DVBC.toString()*/};
                                    
    private static String DeviceId[]  = {"0", "1", "2", "3"};
    private static String Bandwidth[] = {"6000", "7000", "8000"};

    private static MODE mode = MODE.ATSC3;
    private static int  deviceId = 0;
    private static int[] frequency = {538000, 575000, 575000, 575000};
    private static int[] bandwidth = {6000, 6000, 6000, 6000};
    private static int[] plpList = {0, 0, 0, 0};
    private static char[] numPLP = {0};

    public static enum MODE{
        ATSC3(0),
        ATSC1(1),
        DVB_T_T2(2),
        ISDBT(3),
        J83B(4),
        DVBC(5);

        int value;
        MODE(int i) {
            value = i;
        }
        int getValue() {
            return value;
        }
    };

    private static enum POS{
        MODE,
        ID,
        FREQ,
        BW,
    };
    
    private static ProgressDialog progress;
    
    private static final int MESSAGE_INIT           = 0;
    private static final int MESSAGE_ACQUIRECHANNEL = 1;
    private static final int MESSAGE_DEBUG_MSG      = 2;

    public Handler handler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            switch(msg.what)
            {
            case MESSAGE_INIT:
            {
                int tvStandard = msg.arg1;
                Bundle bundle = msg.getData();
                String titleString = bundle.getString("msg");

                progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
                progress.setTitle(titleString);     // Set Title String
                progress.show();

                new Thread(new Runnable(){
                    public void run() {
                        Looper.prepare();
                        long error = Error.Error_NO_ERROR;

                        Message msg = new Message();
                        if(error == Error.Error_NO_ERROR) {
                            //long startTime = SystemClock.uptimeMillis();
                            error = MainActivity.endeavour.init(mode.ordinal());
                            //long endTime = SystemClock.uptimeMillis();
                            //Log.v(TAG, "endeavour init tick = " + (endTime - startTime) + " ms");
    
                            if(error == Error.Error_NO_ERROR) {
                                isInitOk = true;
                            } else {
                                String errMsg = "Endeavour init error = 0x" + Long.toHexString(error);
                                Log.d(TAG, errMsg);
                                msg.what = MESSAGE_DEBUG_MSG;
                                Bundle bundle = new Bundle();
                                bundle.putString("msg", errMsg);
                                msg.setData(bundle);
                                handler.sendMessage(msg);
                            }
                            
                        } if(error == Error.Error_USB_DEVICE_NOT_FOUND) {
                            String errMsg =  "Device isn't be found, error = 0x" + Long.toHexString(error);
                            Log.d(TAG, errMsg);
                            msg.what = MESSAGE_DEBUG_MSG;
                            Bundle bundle = new Bundle();
                            bundle.putString("msg", errMsg);
                            msg.setData(bundle);
                            handler.sendMessage(msg);
                        } else if(error == Error.Error_USB_PID_VID_WRONG) {
                            String errMsg =  "Either PID or VID is wrong = 0x" + Long.toHexString(error);
                            Log.d(TAG, errMsg);
                            msg.what = MESSAGE_DEBUG_MSG;
                            Bundle bundle = new Bundle();
                            bundle.putString("msg", errMsg);
                            msg.setData(bundle);
                            handler.sendMessage(msg);
                        }
                        progress.dismiss();
                    }
                    
                }).start();

                break;
            }
            case MESSAGE_ACQUIRECHANNEL:
            {
                progress.setProgressStyle(ProgressDialog.STYLE_SPINNER);
                progress.setTitle("acquire channel");
                progress.show();
                
                new Thread(new Runnable(){
                    public void run() {
                        Looper.prepare();
                        
                        long err = Error.Error_NO_ERROR;
                        Message msg = new Message();
                        if(mode == MODE.ATSC3) {
                            err = MainActivity.endeavour.TunerDemod_Atsc3Tune(deviceId, frequency[deviceId], bandwidth[deviceId]);
                        } else if (mode == MODE.ATSC1)
                            err = MainActivity.endeavour.TunerDemod_Atsc1Tune(deviceId, frequency[deviceId]);
                        else if (mode == MODE.DVB_T_T2)
                            err = Error.Error_USB_OPEN_UNKNOWN_MODE;
                        else if (mode == MODE.ISDBT)
                            err = Error.Error_USB_OPEN_UNKNOWN_MODE;
                        else if (mode == MODE.J83B)
                            err = Error.Error_USB_OPEN_UNKNOWN_MODE;
                        else if (mode == MODE.DVBC)
                            err = Error.Error_USB_OPEN_UNKNOWN_MODE;
                        else
                            err = Error.Error_USB_OPEN_UNKNOWN_MODE;

                        if (err == 0) {
                            isSetChannel[mode.ordinal()][deviceId] = true;

                            String errMsg = "";
                            if (mode == MODE.ATSC3)
                                errMsg = "SL3000_R855_Atsc3Tune set device = " + deviceId + ", frequency = " + frequency[deviceId] + ", bandwidth = " + bandwidth[deviceId] + ", Lock";
                            else if (mode == MODE.ATSC1)
                                errMsg = "SL3000_R855_Atsc1Tune set device = " + deviceId + ", frequency = " + frequency[deviceId] + ", Lock";
                            else if (mode == MODE.DVB_T_T2)
                                errMsg = "Unknown mode: " + mode.toString();
                            else if (mode == MODE.ISDBT)
                                errMsg = "Unknown mode: " + mode.toString();
                            else if (mode == MODE.J83B)
                                errMsg = "Unknown mode: " + mode.toString();
                            else if (mode == MODE.DVBC)
                                errMsg = "Unknown mode: " + mode.toString();
                            else
                                errMsg = "Unknown mode: " + mode.toString();

                            Log.d(TAG, errMsg);
                            msg.what = MESSAGE_DEBUG_MSG;
                            Bundle bundle = new Bundle();
                            bundle.putString("msg", errMsg);
                            msg.setData(bundle);
                            handler.sendMessage(msg);
                        } else {
                            String errMsg = "";
                            if(mode == MODE.ATSC3)
                                errMsg = "SL3000_R855_Atsc3Tune set device = " + deviceId + ", frequency = " + frequency[deviceId] + ", bandwidth = " + bandwidth[deviceId] + ", unlock. Code = 0x" + Long.toHexString(err);
                            else if (mode == MODE.ATSC1)
                                errMsg = "SL3000_R855_Atsc1Tune set device = " + deviceId + ", frequency = " + frequency[deviceId] + ", unlock. Code = 0x" + Long.toHexString(err);
                            else if (mode == MODE.DVB_T_T2)
                                errMsg = "Unknown mode" + ", error = 0x" + Long.toHexString(err);
                            else if (mode == MODE.ISDBT)
                                errMsg = "Unknown mode" + ", error = 0x" + Long.toHexString(err);
                            else if (mode == MODE.J83B)
                                errMsg = "Unknown mode" + ", error = 0x" + Long.toHexString(err);
                            else if (mode == MODE.DVBC)
                                errMsg = "Unknown mode" + ", error = 0x" + Long.toHexString(err);
                            else
                                errMsg = "Unknown mode" + ", error = 0x" + Long.toHexString(err);

                            Log.d(TAG, errMsg);
                            msg.what = MESSAGE_DEBUG_MSG;
                            Bundle bundle = new Bundle();
                            bundle.putString("msg", errMsg);
                            msg.setData(bundle);
                            handler.sendMessage(msg);
                        }
                        progress.dismiss();
                    }
                }).start();
            
                break;
            }
            case MESSAGE_DEBUG_MSG:
            {
                Bundle bundle = msg.getData();
                String message = bundle.getString("msg");
                Debug.showmessage(getActivity(), message);
                break;
            }
                
            }
        }
    };
    
    public DeviceSet() {
    }
    
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        
        View rootView = inflater.inflate(R.layout.fragment_deviceset, container, false);
        final ListView list = (ListView)rootView.findViewById(R.id.deviceset_list);
        
        InfoAdapter adapter = new InfoAdapter(list.getContext());

        if(mode == MODE.ATSC3) {
            Title = Title_ATSC3;
            Content = Content_ATSC3;
        } else if(mode == MODE.ATSC1){
            Title = Title_ATSC1;
            Content = Content_ATSC1;
        } else if(mode == MODE.DVB_T_T2){
        } else if(mode == MODE.ISDBT){
        } else if(mode == MODE.J83B) {
        } else if(mode == MODE.DVBC) {
        } else {
            Title = Title_ATSC1;
            Content = Content_ATSC1;
        }
        Content[POS.MODE.ordinal()] = mode.toString();
        Content[POS.ID.ordinal()]   = Integer.toString(deviceId);
        Content[POS.FREQ.ordinal()] = Integer.toString(frequency[deviceId]);
        if ((mode == MODE.ATSC3) || (mode == MODE.DVB_T_T2) || (mode == MODE.DVBC))
            Content[POS.BW.ordinal()] = Integer.toString(bandwidth[deviceId]);

        for(int i = 0; i < Title.length; i++)
            adapter.add(new Info(Title[i], Content[i]));

        list.setAdapter(adapter);
        
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {

            @Override
            public void onItemClick(AdapterView<?> parent, View view,
                    int position, long id) {
                // TODO Auto-generated method stub

                POS pos = POS.values()[position];
                switch(pos) {

                    case MODE:
                    {
                        AlertDialog.Builder builder = new AlertDialog.Builder(list.getContext());
                        builder.setTitle("Select Standard");
                        builder.setItems(Mode, new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int item) {
                                mode = MODE.values()[item];
                                String sendStrTitle = new String();
                                InfoAdapter adapter = (InfoAdapter)list.getAdapter();
                                adapter.clear();
                                if(mode == MODE.ATSC3) {
                                    Title   = Title_ATSC3;
                                    Content = Content_ATSC3;

                                    String newMode = Mode[item];
                                    Content[POS.MODE.ordinal()] = newMode;
                                    Content[POS.ID.ordinal()]   = Integer.toString(deviceId);
                                    Content[POS.FREQ.ordinal()] = Integer.toString(frequency[deviceId]);
                                    Content[POS.BW.ordinal()]   = Integer.toString(bandwidth[deviceId]);
                                    sendStrTitle = "Init ATSC 3.0";
                                } else if (mode == MODE.ATSC1) {
                                    Title   = Title_ATSC1;
                                    Content = Content_ATSC1;

                                    String newMode = Mode[item];
                                    Content[POS.MODE.ordinal()] = newMode;
                                    Content[POS.ID.ordinal()]   = Integer.toString(deviceId);
                                    Content[POS.FREQ.ordinal()] = Integer.toString(frequency[deviceId]);
                                    sendStrTitle = "Init ATSC 1.0";
                                } else if (mode == MODE.DVB_T_T2) {
                                    Log.e(TAG, "Unsupported DVB_T_T2 Mode!");
                                } else if (mode == MODE.ISDBT) {
                                    Log.e(TAG, "Unsupported ISDBT Mode!");
                                } else if (mode == MODE.J83B) {
                                    Log.e(TAG, "Unsupported J83B Mode!");
                                } else if (mode == MODE.DVBC) {
                                    Log.e(TAG, "Unsupported DVBC Mode!");
                                } else {
                                    Log.e(TAG, "Unsupported TV Mode!");
                                }
                                
                                for(int i = 0; i < Title.length; i++)
                                    adapter.add(new Info(Title[i], Content[i]));

                                Message msg = new Message();
                                msg.what = MESSAGE_INIT;
                                //msg.arg1 = mode.ordinal();    // Send Standards
                                Bundle bundle = new Bundle();
                                bundle.putString("msg", sendStrTitle); // Send Title String
                                msg.setData(bundle);
                                handler.sendMessage(msg);
                            }

                        });
                        AlertDialog alert = builder.create();
                        alert.show();


                        break;
                    }

                    case ID:
                    {
                        AlertDialog.Builder builder = new AlertDialog.Builder(list.getContext());
                        builder.setTitle("Select ID");
                        builder.setItems(DeviceId, new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int item) {
                                String newId = DeviceId[item];
                                Content[POS.ID.ordinal()] = newId;
                                deviceId = Integer.valueOf(newId);
                                Content[POS.FREQ.ordinal()] = Integer.toString(frequency[deviceId]);
                                if((mode == MODE.ATSC3) || (mode == MODE.DVB_T_T2) || (mode == MODE.DVBC))
                                    Content[POS.BW.ordinal()] = Integer.toString(bandwidth[deviceId]);
                                InfoAdapter adapter = (InfoAdapter)list.getAdapter();
                                adapter.clear();
                                for(int i = 0; i < Title.length; i++)
                                    adapter.add(new Info(Title[i], Content[i]));
                            }
                        });
                        AlertDialog alert = builder.create();
                        alert.show();
                        break;
                    }

                    case FREQ:
                    {
                        AlertDialog.Builder builder = new AlertDialog.Builder(list.getContext());
                        builder.setTitle("Set Frequency(KHz)");
                        final EditText input = new EditText(list.getContext());
                        input.setInputType(InputType.TYPE_CLASS_NUMBER | InputType.TYPE_NUMBER_VARIATION_NORMAL);
                        builder.setView(input);
                        builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int whichButton) {
                                String newFrequency = input.getText().toString();
                                Content[POS.FREQ.ordinal()] = newFrequency;
                                frequency[deviceId] = Integer.valueOf(newFrequency);
                                InfoAdapter adapter = (InfoAdapter)list.getAdapter();
                                adapter.clear();
                                for(int i = 0; i < Title.length; i++)
                                    adapter.add(new Info(Title[i], Content[i]));
                            }
                        });
                        AlertDialog alert = builder.create();
                        alert.show();
                        break;
                    }

                    case BW:
                    {
                        AlertDialog.Builder builder = new AlertDialog.Builder(list.getContext());
                        builder.setTitle("Select Bandwidth");
                        builder.setItems(Bandwidth, new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int item) {
                                String newBandwidth = Bandwidth[item];
                                Content[POS.BW.ordinal()] = newBandwidth;
                                bandwidth[deviceId] = Integer.valueOf(newBandwidth);
                                InfoAdapter adapter = (InfoAdapter)list.getAdapter();
                                adapter.clear();
                                for(int i = 0; i < Title.length; i++)
                                    adapter.add(new Info(Title[i], Content[i]));
                            }
                        });
                        AlertDialog alert = builder.create();
                        alert.show();
                        break;
                    }
                }
            }
        });
        
        Button apply = (Button)rootView.findViewById(R.id.deviceset_apply);
        
        apply.setOnClickListener(new View.OnClickListener() {
            
            @Override
            public void onClick(View v) {
                Message msg = new Message();
                msg.what = MESSAGE_ACQUIRECHANNEL;
                handler.sendMessage(msg);
            }
        });
        
        progress = new ProgressDialog(getActivity());

        if((MainActivity.isOpen  == true) && (isInitOk == false)) {
            String sendStrTitle = new String();
            Message msg = new Message();
            sendStrTitle = "Driver Inititalize";  // Set Title String
            msg.what = MESSAGE_INIT;                // Set message command
            //msg.arg1 = 1;             // Set Init Standard from global variable "mode"
            Bundle bundle = new Bundle();
            bundle.putString("msg", sendStrTitle);
            msg.setData(bundle);
            handler.sendMessage(msg);
        }
        
        return rootView;
    }
    
    @Override
    public void onStop()
    {
        Log.v(TAG, Debug.getLineInfo());
        super.onStop();
    }
    
    private class Info
    {
        String title;
        String content;
        
        public Info(String t, String c)
        {
            title = t;
            content = c;
        }
    }
     
    private class InfoAdapter extends ArrayAdapter<Info> {
        LayoutInflater inflator;
 
        public InfoAdapter(Context context) {
            super(context, 0);
            inflator = LayoutInflater.from(context);
        }
 
        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = inflator.inflate(
                        android.R.layout.simple_list_item_2, parent, false);
            }
 
            TextView text1 = (TextView) convertView
                    .findViewById(android.R.id.text1);
            TextView text2 = (TextView) convertView
                    .findViewById(android.R.id.text2);
 
            Info s = this.getItem(position);
 
            text1.setText(s.title);
            text2.setText("" + s.content);
 
            return convertView;
        }
    }

}
