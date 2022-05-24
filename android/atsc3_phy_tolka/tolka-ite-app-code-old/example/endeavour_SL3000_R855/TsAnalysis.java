package com.example.endeavour_SL3000_R855;

import com.api.Bus;
import com.api.Debug;
import com.api.Register;
import com.api.User;

import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.util.Arrays;
import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.FileInputStream;
import java.io.File;
import java.io.RandomAccessFile;

import android.support.v4.app.Fragment;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.os.Process;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.GridView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

public class TsAnalysis extends Fragment{
    private static final String TAG = "Endeavour.TsAnalysis";
    
    private static final int MAX_PID_NUM = 64;
    private static TsStatus []ts_status = new TsStatus[MAX_PID_NUM];
    private static int PID_num = 0;
    private static final int DROP_DATA_LIMIT = 50;
    private static int drop_data_time=0;

    private static int []PIDFilterList = new int[MAX_PID_NUM];
    private static int num_inPIDFilterList = 0;
    
    private static final int TS_STATUS_UPDATE = 0x0001;
    private static final int TS_STATUS_CLEAR = 0x0002;
    private static final int TS_PARSER = 0x0003;
    protected static final ByteBuffer ByteBuffer = null;
    private static View view = null;
    private static boolean already_GetTs = false;
    private static boolean get_cpu_status = false;
    private static Thread thread;
    private static GridView Grid_status_2;
    private static GridView Grid_status;
    private static InfoAdapter adapter;
    private static InfoAdapter adapter_2;
    private static long Elapsed_Time;
    private static long total_packets;
    private static long total_kbitsrate;
    
    private static final int sync_byte[] = {0x40, 0x41, 0x42, 0x43};
    private static int deviceId = 0;
    private static byte[] ts_buffer0 = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER];
    private static byte[] ts_buffer1 = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER];
    private static byte[] ts_buffer2 = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER];
    private static byte[] ts_buffer3 = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER];
    private static int ts_buffer0_length = 0;
    private static int ts_buffer1_length = 0;
    private static int ts_buffer2_length = 0;
    private static int ts_buffer3_length = 0;

    private byte[] big_buf     = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER + 188];
    private int    big_buf_len = 0;    
    private byte[] tmp_buf     = new byte[188];
    private int    tmp_buf_len = 0;

    private int frame_cnt = 0;
    private int parse_cnt = 0;
    private float cpu_usage = 0;


    //jjustman-2022-05-19 - capture our tlv payload to our local directory, e.g.
    //run-as com.example.endeavour_SL3000_R855.debug
    //download using android studio device file explorer

    File file ;



    @Override
    public void onCreate(Bundle savedInstanceState) {

		if(User.TUNER_CHANNEL == 1)
	        sync_byte[0] = 0x47;
		else
			sync_byte[0] = 0x40;


        file = new File(getContext().getFilesDir(), "sl.tlv");


        super.onCreate(savedInstanceState);
    }
    
    private long GetOvlCnt()
    {
        long uipOvlCnt = 0;
        int[] ucOvlCntLow = new int[1];
        int[] ucOvlCntHigh = new int[1];
        MainActivity.endeavour.IT9300_readRegister(0, Register.r_br_mp2if_lost_pkt_cnt_l, ucOvlCntLow);
        MainActivity.endeavour.IT9300_readRegister(0, Register.r_br_mp2if_lost_pkt_cnt_h, ucOvlCntHigh);
      
        uipOvlCnt = (((int)ucOvlCntHigh[0])<<8) + ucOvlCntLow[0];   
        return uipOvlCnt;
    }
    
    private void UpdateTsStatus(TsStatus ts_status, InfoAdapter adapter)
    {

        if(ts_status.PIDFilterLock)
           adapter.add(new Info(Long.toString(ts_status.PID), "Lock"));
        else
           adapter.add(new Info(Long.toString(ts_status.PID), ""));                        
       
        adapter.add(new Info(Long.toString(ts_status.Packets_counter), ""));                    
        adapter.add(new Info(Long.toString(ts_status.packet_loss_num), ""));
        adapter.add(new Info(Long.toString(ts_status.packet_error_num), ""));
        adapter.add(new Info(Long.toString(ts_status.KBitsRate), ""));
    }

    long  max_bound = 0, avg_bound=0, i=0;
    long  sum=0;

    private Handler handler = new Handler()
    {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case TS_PARSER:         
                    
                    Bundle bundle = msg.getData();
                            
                    int[] bufferLength = bundle.getIntArray("bufferLength");
                    byte[] buffer = bundle.getByteArray("buffer");
                    
                    ParseTsFrame(bufferLength[0], buffer);                  
                    
                    break;
                case TS_STATUS_CLEAR:
                   Init_adapter(adapter, title);
                   Init_adapter(adapter_2, title_2);

                   break;
                case TS_STATUS_UPDATE:
                   int[] temp = {1};
                   int[] tmp0 = {0};
                   int[] tmp1 = {0};
                   int    data_cnt;
                   
                   adapter.clear();
                   adapter_2.clear();

                   MainActivity.endeavour.IT9300_readRegister(0, Register.p_br_reg_ts0_overflow, temp);

                   MainActivity.endeavour.IT9300_readRegister(0, Register.r_br_mp2if_psb_count_7_0, tmp0);
                   MainActivity.endeavour.IT9300_readRegister(0, Register.r_br_mp2if_psb_count_12_8, tmp1);
                   data_cnt = 4*((tmp1[0] * 256) + tmp0[0]);
       //            Log.d(TAG, "data count: " + data_cnt);

                    i++;
                    sum = sum + data_cnt;
                    avg_bound = sum / i;
/*                    if(i >= 100) {
                        avg_bound /= i;
                        i=0;
                    }*/
                    if(data_cnt > max_bound)
                        max_bound = data_cnt;

                   if((temp[0] & 0x01) == 1) {
                       adapter_2.add(new Info(title_2[0], "OVERFLOW"));
//                       adapter_2.add(new Info(title_2[0], "OVERFLOW" + "(" + data_cnt + ")" + "MAX_bound:(" + max_bound + "), avg_bound:(" + avg_bound + ")"));
                       MainActivity.endeavour.IT9300_writeRegister(0, Register.p_br_reg_ts0_overflow, temp[0]);
                   } else {
                       adapter_2.add(new Info(title_2[0], "EMPTY " + "(" + data_cnt + ")" ));
//                       adapter_2.add(new Info(title_2[0], "EMPTY " + "(" + data_cnt + ")" + "MAX_bound:(" + max_bound + "), avg_bound:(" + avg_bound + ")" ));
                   }


                   
                   adapter_2.add(new Info(title_2[1], Long.toString(GetOvlCnt())));
                   adapter_2.add(new Info(title_2[2], Long.toString(total_kbitsrate)));
                   adapter_2.add(new Info(title_2[3], Integer.toString(Math.round(cpu_usage)) + "%"));
                   
                   adapter_2.notifyDataSetChanged();
                   adapter_2.setNotifyOnChange(true);
                   
                   for(int i = 0; i < title.length; i++)
                      adapter.add(new Info(title[i], ""));
                   
                   long []Packets_counter_array = new long[MAX_PID_NUM];
                   long []Packets_counter_array_org = new long[MAX_PID_NUM];
                   for(int i = 0; i < MAX_PID_NUM; i++)
                   {
                       if(i<PID_num)
                       {
                           Packets_counter_array[i] = ts_status[i].Packets_counter;
                           //Do not write "Packets_counter_array_org[i] = ts_status[i].Packets_counter" for data synchronization    
                            
                       }
                       else                   
                           Packets_counter_array[i] = 0;                           
                     
                       Packets_counter_array_org[i] = Packets_counter_array[i];
                   }
                   
                   Arrays.sort(Packets_counter_array); 

                   for(int i=(MAX_PID_NUM-1); i>=(MAX_PID_NUM-PID_num); i--)
                   {
                       for(int k = 0; k<PID_num; k++)
                       {
                           if(Packets_counter_array[i] == Packets_counter_array_org[k])
                           {
                               Packets_counter_array_org[k] = -1;
                               
                               if(num_inPIDFilterList != 0)
                               {
                                   for(int j=0; j<num_inPIDFilterList; j++)
                                   {
                                       if(ts_status[k].PID == PIDFilterList[j])                                   
                                           UpdateTsStatus(ts_status[k], adapter);
                                   }
                               }
                               else
                                   UpdateTsStatus(ts_status[k], adapter);                                  
                                                               
                               break;
                           }        
                       }
                   }
                    
                   adapter.notifyDataSetChanged();
                   adapter.setNotifyOnChange(true);
                    
                   break;
                    
            }   
            super.handleMessage(msg);   
        }   
    };
    
    private void Calculate_KBitsRate()
    {
        //make sure that don't divide by zero
        for(int i = 0; i < PID_num; i++)
            ts_status[i].KBitsRate = ts_status[i].Packets_counter*188*8 /(Elapsed_Time);
        Log.d(TAG, "JJ: total packet: " + total_packets);
        total_kbitsrate = total_packets*188*8 / (Elapsed_Time);
    }

    private void Continuity_counter_Anticipation(TsStatus ts_status)
    {
        if(ts_status.Continuity_counter == 0xF)
            ts_status.Continuity_counter = 0x0;
        else
            ts_status.Continuity_counter = ts_status.Continuity_counter +1;     
    }

    private void ParseTsFrame(int TS_Len, byte[] TS_buffer)
    {
        int i, j;
        boolean found = false;

        parse_cnt = parse_cnt + 1;
//        Log.d(TAG, "parse count: " + parse_cnt + ", first byte: " + Integer.toHexString(TS_buffer[0]));

        try {
            Arrays.fill(ts_buffer0, (byte) 0);
            Arrays.fill(ts_buffer1, (byte) 0);
            Arrays.fill(ts_buffer2, (byte) 0);
            Arrays.fill(ts_buffer3, (byte) 0);

            ts_buffer0_length = 0;
            ts_buffer1_length = 0;
            ts_buffer2_length = 0;
            ts_buffer3_length = 0;

            //magic? 0x24681357
            if(MainActivity.channels == 1){		// 1 channels
                big_buf_len = 0;
                ts_buffer0_length = 0;

                // cascate current frame and the last part of the previous frame
                // copy small piece of data (last part of the previous frame)
                            Log.d(TAG, "TS_Len: " + TS_Len);
                            Log.d(TAG, "TS_tmp_buf_len: " + tmp_buf_len);
                if (tmp_buf_len > 0) {
                    System.arraycopy(tmp_buf, 0, big_buf, 0, tmp_buf_len);
                    tmp_buf_len = 0;
                    big_buf_len = tmp_buf_len;
                    //                Log.d(TAG, "tmp_buf_len: " + tmp_buf_len + "big_buf_len: " + big_buf_len);
                }

                // copy big piece of data (current frame)
                //            Log.d(TAG, "frame size: " + User.IT9300User_USB20_FRAME_SIZE);
                System.arraycopy(TS_buffer, 0, big_buf, big_buf_len, User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER);
                big_buf_len = big_buf_len + User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER;
                //            Log.d(TAG, "big_buf_len: " + big_buf_len);

                // extract TS packets and drop garbage data
                i = 0;
                while(i < big_buf_len) {
                    //Log.d(TAG, "i(B): " + i + "sync b: 0x" + Integer.toHexString(big_buf[i]));
                    if(big_buf[i] != 0x47)
                    {
                        j = 0;
                        found = false;
                        while(i + j + 188 < big_buf_len) {
                            if((big_buf[i+j] == 0x47) && (big_buf[i+j+188] == 0x47)) {
                                found = true;
                                //jjustman-2022-05-19 - is this right? ...no
                                //
                                //i = i + j;
                                break;
                            }
                            j = j + 1;
                        }
                        if (found) {
                                                   Log.d(TAG, "ts_found");
                            i = i + j;
                        }
                        else {
                            //                       Log.d(TAG, "not found");
                            i = big_buf_len;
                        }
                        //                   Log.d(TAG, "i: " + i + "j: " + j);
                    }
                    else
                    {
                        int tempLen = big_buf_len - i - 188;
                        if((tempLen < 188) && (tempLen != 0) ) {
                            System.arraycopy(big_buf, i + 188, tmp_buf, 0, tempLen);
                            tmp_buf_len = tempLen;
                            i = i + tempLen;
                            //                       Log.d(TAG, "tempLen: " + tempLen);
                        }
                        else {
                            if ((ts_buffer0_length + 188) > User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER) {
                                Log.e(TAG, "dest buffer is too samll for saving data!!!");
                                return;
                            }
                            Log.d(TAG, "ts_extract OK, i: " + i);
                            System.arraycopy(big_buf, i, ts_buffer0, ts_buffer0_length, 188);
                            Log.d(TAG, String.format("ts_buffer0 idx[%d]=%d, idx[%d]=%d", ts_buffer0_length, ts_buffer0[ts_buffer0_length], ts_buffer0_length+1, ts_buffer0[ts_buffer0_length+1]));
                            ts_buffer0_length += 188;
                            i = i + 188;
                        }
                    }
                    //Log.d(TAG, "i(E): " + i);
                }
                Log.d(TAG, "ts_buffer0_length: " + ts_buffer0_length);
            } else {		// 2, 4 channels

                for(j = 0; j < TS_Len; j += 188)
                {
                    if(MainActivity.channels == 4) {
                        if((TS_buffer[j] != sync_byte[0]) &&
                                (TS_buffer[j] != sync_byte[1]) &&
                                (TS_buffer[j] != sync_byte[2]) &&
                                (TS_buffer[j] != sync_byte[3]))
                        {
                            for(int k = 0; k < 188; k++)
                            {
                                if((j + k >= TS_Len) || (j + k + 188 >= TS_Len))
                                    break;

                                if(((TS_buffer[j + k] == sync_byte[0]) ||
                                        (TS_buffer[j + k] == sync_byte[1]) ||
                                        (TS_buffer[j + k] == sync_byte[2]) ||
                                        (TS_buffer[j + k] == sync_byte[3]))
                                        &&
                                        ((TS_buffer[j + k + 188] == sync_byte[0]) ||
                                                (TS_buffer[j + k + 188] == sync_byte[1]) ||
                                                (TS_buffer[j + k + 188] == sync_byte[2]) ||
                                                (TS_buffer[j + k + 188] == sync_byte[3])))
                                {
                                    j = j + k;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if(TS_buffer[j] == sync_byte[0]) {
                                System.arraycopy(TS_buffer, j, ts_buffer0, ts_buffer0_length, 188);
                                ts_buffer0_length += 188;
                            } else if(TS_buffer[j] == sync_byte[1]) {
                                System.arraycopy(TS_buffer, j, ts_buffer1, ts_buffer1_length, 188);
                                ts_buffer1_length += 188;
                            } else if(TS_buffer[j] == sync_byte[2]) {
                                System.arraycopy(TS_buffer, j, ts_buffer2, ts_buffer2_length, 188);
                                ts_buffer2_length += 188;
                            } else if(TS_buffer[j] == sync_byte[3]) {
                                System.arraycopy(TS_buffer, j, ts_buffer3, ts_buffer3_length, 188);
                                ts_buffer3_length += 188;
                            }

                            int tempLen = TS_Len - j - 188;
                            if((tempLen < 188) && (tempLen != 0) ) {
                                if(TS_buffer[j] == sync_byte[0]) {
                                    System.arraycopy(TS_buffer, j + 188, ts_buffer0, ts_buffer0_length, tempLen);
                                    ts_buffer0_length += tempLen;
                                } else if(TS_buffer[j] == sync_byte[1]) {
                                    System.arraycopy(TS_buffer, j + 188, ts_buffer1, ts_buffer1_length, tempLen);
                                    ts_buffer1_length += tempLen;
                                } else if(TS_buffer[j] == sync_byte[2]) {
                                    System.arraycopy(TS_buffer, j + 188, ts_buffer2, ts_buffer2_length, tempLen);
                                    ts_buffer2_length += tempLen;
                                } else if(TS_buffer[j] == sync_byte[3]) {
                                    System.arraycopy(TS_buffer, j + 188, ts_buffer3, ts_buffer3_length, tempLen);
                                    ts_buffer3_length += tempLen;
                                }
                            }
                        }
                    }
                    else if(MainActivity.channels == 2) {
                        if((TS_buffer[j] != sync_byte[0]) &&
                                (TS_buffer[j] != sync_byte[1]))
                        {
                            for(int k = 0; k < 188; k++)
                            {
                                if((j + k >= TS_Len) || (j + k + 188 >= TS_Len))
                                    break;

                                if(((TS_buffer[j + k] == sync_byte[0]) ||
                                        (TS_buffer[j + k] == sync_byte[1]))
                                        &&
                                        ((TS_buffer[j + k + 188] == sync_byte[0]) ||
                                                (TS_buffer[j + k + 188] == sync_byte[1])))
                                {
                                    j = j + k;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if(TS_buffer[j] == sync_byte[0]) {
                                System.arraycopy(TS_buffer, j, ts_buffer0, ts_buffer0_length, 188);
                                ts_buffer0_length += 188;
                            } else if(TS_buffer[j] == sync_byte[1]) {
                                System.arraycopy(TS_buffer, j, ts_buffer1, ts_buffer1_length, 188);
                                ts_buffer1_length += 188;
                            }

                            int tempLen = TS_Len - j - 188;
                            if((tempLen < 188) && (tempLen != 0) ) {
                                if(TS_buffer[j] == sync_byte[0]) {
                                    System.arraycopy(TS_buffer, j + 188, ts_buffer0, ts_buffer0_length, tempLen);
                                    ts_buffer0_length += tempLen;
                                } else if(TS_buffer[j] == sync_byte[1]) {
                                    System.arraycopy(TS_buffer, j + 188, ts_buffer1, ts_buffer1_length, tempLen);
                                    ts_buffer1_length += tempLen;
                                }
                            }
                        }
                    }


    /*                    if(TS_buffer[j] != 0x47)
                        {
                            for(int k = 0; k < 188; k++)
                            {
                                if((j + k >= TS_Len) || (j + k + 188 >= TS_Len))
                                    break;

                                if((TS_buffer[j+k] == 0x47) && (TS_buffer[j+k+188] == 0x47))
                                {
                                    j = j + k;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            System.arraycopy(TS_buffer, j, ts_buffer0, ts_buffer0_length, 188);
                            ts_buffer0_length += 188;

                            int tempLen = TS_Len - j - 188;
                            if((tempLen < 188) && (tempLen != 0) ) {
                                System.arraycopy(TS_buffer, j + 188, ts_buffer0, ts_buffer0_length, tempLen);
                                ts_buffer0_length += tempLen;
                            }
                        }*/
                    else {
                        Log.e(TAG, "Unsopported number of channels!");
                    }
                }


            }

                byte[] ts_buffer = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER];
                int ts_buffer_length = 0;
                if(MainActivity.channels == 4) {
                    if(deviceId == 0) {
                        ts_buffer        = ts_buffer0;
                        ts_buffer_length = ts_buffer0_length;
                    } else if(deviceId == 1) {
                        ts_buffer        = ts_buffer1;
                        ts_buffer_length = ts_buffer1_length;
                    } else if(deviceId == 2) {
                        ts_buffer        = ts_buffer2;
                        ts_buffer_length = ts_buffer2_length;
                    } else if(deviceId == 3) {
                        ts_buffer        = ts_buffer3;
                        ts_buffer_length = ts_buffer3_length;
                    }
                }
                else if(MainActivity.channels == 2) {
                    if(deviceId == 0) {
                        ts_buffer        = ts_buffer0;
                        ts_buffer_length = ts_buffer0_length;
                    } else if(deviceId == 1) {
                        ts_buffer        = ts_buffer1;
                        ts_buffer_length = ts_buffer1_length;
                    }
                }
                else if (MainActivity.channels == 1) {
                    ts_buffer        = ts_buffer0;
                    ts_buffer_length = ts_buffer0_length;


                    //jjustman-2022-05-19 - persist this to a tlv capture

                    FileChannel wChannel = new FileOutputStream(file, true).getChannel();

                    wChannel.write(java.nio.ByteBuffer.wrap(ts_buffer));
                }
                else {
                    Log.e(TAG, "unsupported number of channels!");
                }

                for(i = 0; i < ts_buffer_length; i += 188)
                {
                    int packet_pid = ((ts_buffer[i + 1] & 0x1F) * 256 + (ts_buffer[i + 2] & 0xFF));

                    if(num_inPIDFilterList != 0)
                    {
                        for(int g = 0; g < num_inPIDFilterList; g++)
                        {
                            if(packet_pid == PIDFilterList[g])
                            {
                                total_packets++;
                                break;
                            }
                        }
                    }
                    else
                        total_packets++;

                                    if(packet_pid == 0x1FFF) //ignore null packets
                                        continue;

                    if(PID_num == 0)    // fist TS packet
                    {
                        if(ts_status[0].PID == -1)
                        {
                            ts_status[0].PID = packet_pid;
                            ts_status[0].Packets_counter++;

                            ts_status[0].Continuity_counter = (ts_buffer[i + 3] & 0xF);

                            if( (ts_buffer[i + 1] & 0x80) == 0x80)
                                ts_status[0].packet_error_num++;

                            Continuity_counter_Anticipation(ts_status[0]);

                            PID_num = 1;
                            continue;
                        }
                    }

                    int find_loop;
                    for(find_loop = 0; find_loop < PID_num; find_loop++)
                    {
                        if(ts_status[find_loop].PID == packet_pid)
                        {
                            ts_status[find_loop].Packets_counter++;

                            if( (ts_buffer[i + 1] & 0x80) == 0x80)
                                ts_status[find_loop].packet_error_num++;

                            //check "Adaptation field exist",
                            //10 = "adaptation field only", won't Incremented Continuity counter
                            if((ts_buffer[i + 3] & 0x30) == 0x20) {
                                //                           Log.d(TAG, "adaptation only (" + ts_status[find_loop].Continuity_counter +")");
                                break;
                            }

                            if( (ts_status[find_loop].Continuity_counter & 0xF) != (ts_buffer[i + 3] & 0xF))
                            {
                                long packet_num_real = (ts_buffer[i + 3] & 0xF);
                                long packet_num_ideal = (ts_status[find_loop].Continuity_counter & 0xF);
                                //                        Log.d(TAG, "real: " + packet_num_real + ", expected: " + packet_num_ideal);
                                ts_status[find_loop].packet_loss_times++;

                                //                          Log.d(TAG, "prev loss: " + ts_status[find_loop].packet_loss_num);
                                if(packet_num_real > packet_num_ideal)
                                    ts_status[find_loop].packet_loss_num += (packet_num_real - packet_num_ideal);
                                else if(packet_num_real < packet_num_ideal)
                                    ts_status[find_loop].packet_loss_num += (0xF - packet_num_ideal + 1 + packet_num_real);
                                else
                                    Log.v(TAG, "packet_num_real == packet_num_ideal ???");
                                //                           Log.d(TAG, "curr loss: " + ts_status[find_loop].packet_loss_num);

                                ts_status[find_loop].Continuity_counter = packet_num_real;
                            }

                            Continuity_counter_Anticipation(ts_status[find_loop]);

                            break;
                        }
                    }

                    if((find_loop == PID_num) && (PID_num < MAX_PID_NUM))
                    {
                        if(ts_status[PID_num].PID == -1)
                        {
                            ts_status[PID_num].PID = packet_pid;

                            ts_status[PID_num].Packets_counter++;

                            ts_status[PID_num].Continuity_counter = (ts_buffer[i + 3] & 0xF);

                            if( (ts_buffer[i + 1] & 0x80) == 0x80)
                                ts_status[PID_num].packet_error_num++;

                            Continuity_counter_Anticipation(ts_status[PID_num]);

                            PID_num++;
                        }
                    }
                    //log("TS_buffer["+i+"]= "+Integer.toHexString( 0xff & TS_buffer[i] ));
                }
        } catch (Exception e) {
            Log.w(TAG, "Exception:" +e);
        }
    }

    class GetTSThread extends Thread {
        
            @Override
            public void run() {
                //BUFFER_STATUS_OVERFLOW means that we have got TS data
                //if((temp[0] & 0x1) == 1)
                {

                    
                    PID_num = 0;
                    Elapsed_Time = 0;
                    long TimeCheck = 0;
                    total_packets = 0;
                
                    if(num_inPIDFilterList>0)
                    {
                        try {
                            Thread.sleep(1000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }                       
                    }
                    
                    //ts_status initialization
                    for(int i=0; i<MAX_PID_NUM; i++)
                        ts_status[i] = new TsStatus();
                    
                    TimeCheck = SystemClock.uptimeMillis();

                    while(already_GetTs)
                    {                       
                        byte[] buffer = new byte[User.IT9300User_USB20_FRAME_SIZE * User.IT9300User_USB20_FRAME_NUNBER];
                        int[] bufferLength = new int[1];

                        if (Bus.Datagram.getTs(buffer, bufferLength, User.IT9300User_USB20_FRAME_NUNBER)) {
                            Log.d(TAG, String.format("TS_thread: getTs size is: %d", bufferLength[0]));
                            if(drop_data_time < DROP_DATA_LIMIT ) {     // Drop first data
                                drop_data_time++;
                            } else {
                                long current_time = SystemClock.uptimeMillis();
                                //update ts status every sec
                                if ((current_time - TimeCheck) >= 500) {
                                    Elapsed_Time += (current_time - TimeCheck);

                                    Message msg = new Message();

                                    Calculate_KBitsRate();
                                    msg.what = TS_STATUS_UPDATE;
                                    handler.sendMessage(msg);
                                    TimeCheck = SystemClock.uptimeMillis();
                                }
	                            ParseTsFrame(bufferLength[0], buffer);
                            }
                        }
                    }
                }
            }
    }
    class GetCPUUsageThread extends Thread {
        public float getProcessCpuRate()
        {

            float totalCpuTime1 = getTotalCpuTime();
            float processCpuTime1 = getAppCpuTime();
            try
            {
                Thread.sleep(360);
            }
            catch (Exception e)
            {
            }

            float totalCpuTime2 = getTotalCpuTime();
            float processCpuTime2 = getAppCpuTime();

            float cpuRate = 100 * (processCpuTime2 - processCpuTime1)
                    / (totalCpuTime2 - totalCpuTime1);

            return cpuRate;
        }

        public long getTotalCpuTime()
        { // Get system total time of cpu
            String[] cpuInfos = null;
            long totalCpu = 0L;
            try
            {
                BufferedReader reader = new BufferedReader(new InputStreamReader(
                        new FileInputStream("/proc/stat")), 1000);
                String load = reader.readLine();
                reader.close();
                cpuInfos = load.split(" ");

                totalCpu = Long.parseLong(cpuInfos[2])
                    + Long.parseLong(cpuInfos[3]) + Long.parseLong(cpuInfos[4])
                    + Long.parseLong(cpuInfos[6]) + Long.parseLong(cpuInfos[5])
                    + Long.parseLong(cpuInfos[7]) + Long.parseLong(cpuInfos[8]);

            }
            catch (IOException ex)
            {
             //   ex.printStackTrace();
            }
            return totalCpu;
        }

        public long getAppCpuTime()
        { // get application time for all of the cpu
            String[] cpuInfos = null;
            try
            {
                int pid = android.os.Process.myPid();
                BufferedReader reader = new BufferedReader(new InputStreamReader(
                        new FileInputStream("/proc/" + pid + "/stat")), 1000);
                String load = reader.readLine();
                reader.close();
                cpuInfos = load.split(" ");
            }
            catch (IOException ex)
            {
                ex.printStackTrace();
            }
            long appCpuTime = Long.parseLong(cpuInfos[13])
                    + Long.parseLong(cpuInfos[14]) + Long.parseLong(cpuInfos[15])
                    + Long.parseLong(cpuInfos[16]);
            return appCpuTime;
        }

        @Override
        public void run() {

/*            Log.d(TAG, "TID:"+ Process.getThreadPriority(android.os.Process.myTid()));
            Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);    // -19
            Log.d(TAG, "TID2:"+ Process.getThreadPriority(android.os.Process.myTid()));
            Looper.prepare();*/
            while(true) {
                cpu_usage = getProcessCpuRate();
                try
                {
                    Thread.sleep(500);
                }
                catch (Exception e)
                {
                }
            }
        }
    }

    private void Init_adapter(InfoAdapter adapter, String[] title)
    {
        adapter.clear();
        //title setting 
        for(int i = 0; i < title.length; i++)
            adapter.add(new Info(title[i], ""));
        
    }
    
    public static ByteBuffer clone(ByteBuffer original) {
        ByteBuffer clone = java.nio.ByteBuffer.allocate(original.capacity()); 
        original.rewind();//copy from the beginning 
        clone.put(original); 
        original.rewind(); 
        clone.flip(); 
        return clone; 
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInsatnceState)
    {
        Log.v(TAG, Debug.getLineInfo());
            
        view = inflater.inflate(R.layout.fragment_tsanalysis, container, false);    
        Grid_status_2 = (GridView) view.findViewById(R.id.TsAnalysislistView2); //must be final type because of local variable
        Grid_status = (GridView) view.findViewById(R.id.TsAnalysislistView);
        Grid_status.setNumColumns(5);
        Grid_status_2.setNumColumns(3);
        adapter = new InfoAdapter(Grid_status.getContext()); 
        adapter_2 = new InfoAdapter(Grid_status_2.getContext());        
        
        total_kbitsrate = 0;
        
        Init_adapter(adapter, title);
        Init_adapter(adapter_2, title_2);   
       
        Grid_status.setAdapter(adapter);      
        Grid_status_2.setAdapter(adapter_2);
        
        num_inPIDFilterList = 0;
        
        RadioGroup deviceGroup = (RadioGroup)view.findViewById(R.id.TsAnalysis_device_group);
        deviceGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                // TODO Auto-generated method stub
                
                already_GetTs = false;
                try
                {
                    Thread.sleep(100);
                }
                catch (Exception e)
                {}
                Message msg = new Message();
                msg.what = TS_STATUS_CLEAR;
                handler.sendMessage(msg);
                
                switch(checkedId)
                {
                case R.id.TsAnalysis_device_0:
                    Log.d(TAG, "device_0");
                    deviceId = 0;
                    break;
                case R.id.TsAnalysis_device_1:
                    Log.d(TAG, "device_1");
                    deviceId = 1;
                    break;
                case R.id.TsAnalysis_device_2:
                    Log.d(TAG, "device_2");
                    deviceId = 2;
                    break;
                case R.id.TsAnalysis_device_3:
                    Log.d(TAG, "device_3");
                    deviceId = 3;
                    break;
                }
                
                already_GetTs = true;
                drop_data_time = 0;
                thread = new GetTSThread();

                // ATSC3 PLP Test sample code
                long err;
                char PLPMask = 0x0F;    // PLP0 -> PLP3 all enable.
//                char[] PLPIDList = {0x00, 0x01, 0x02, 0x03};
                char[] PLPIDList = {0x00, 0xFF, 0xFF, 0xFF};

                Log.d(TAG, "===== TEST TunerDemod_Atsc3SetPLP =====");
                err = MainActivity.endeavour.TunerDemod_Atsc3SetPLP(deviceId, PLPMask, PLPIDList);
                if (err != 0)
                {
                    Log.d(TAG, "Error : TunerDemod_Atsc3SetPLP failed. (err = "+ err + ")");
                }

/*
                // SL3000 Read / Write Register Test sample
                long retVal;
                // Read version number
                int  regLen = 4;
                byte[]  swMajorNo = new byte[regLen];
                byte[]  swMinorNo = new byte[regLen];

                retVal = MainActivity.endeavour.Demod_Atsc1ReadRegisters(deviceId, 0x00000024, regLen, swMajorNo);      // GET_SW_MAJOR_NUM: 0x00000024
                Log.d(TAG, "First read swMajorNo: 0x" + Integer.toHexString(swMajorNo[0] & 0xFF) + ", 0x" + Integer.toHexString(swMajorNo[1] & 0xFF) + ", 0x" + Integer.toHexString(swMajorNo[2] & 0xFF) + ", 0x" + Integer.toHexString(swMajorNo[3] & 0xFF));
                if (retVal == 0)
                {
                    retVal = MainActivity.endeavour.Demod_Atsc1ReadRegisters(deviceId, 0x00000028, regLen, swMinorNo);      //GET_SW_MINOR_NUM: 0x00000028
                    Log.d(TAG, "First read swMinorNo: 0x" + Integer.toHexString(swMinorNo[0] & 0xFF) + ", 0x" + Integer.toHexString(swMinorNo[1] & 0xFF) + ", 0x" + Integer.toHexString(swMinorNo[2] & 0xFF) + ", 0x" + Integer.toHexString(swMinorNo[3] & 0xFF));
                }


                // Read SET_DEMOD_STOP address value
                byte[] setVal = new byte[regLen];
                retVal = MainActivity.endeavour.Demod_Atsc1ReadRegisters(deviceId, 0x0000002C, regLen, setVal);     // SET_DEMOD_STOP: 0x0000002C
                if (retVal == 0)
                {
                    Log.d(TAG, "1. read SET_DEMOD_STOP: 0x" + Integer.toHexString(setVal[0] & 0xFF) + ", 0x" + Integer.toHexString(setVal[1] & 0xFF) + ", 0x" + Integer.toHexString(setVal[2] & 0xFF) + ", 0x" + Integer.toHexString(setVal[3] & 0xFF));
                }
                // Enable SET_DEMOD_STOP
                setVal[0] = (byte) 0xFF;
                setVal[1] = (byte) 0x00;
                setVal[2] = (byte) 0x00;
                setVal[3] = (byte) 0x00;
                retVal = MainActivity.endeavour.Demod_Atsc1WriteRegisters(deviceId, 0x0000002C, regLen, setVal);    // SET_DEMOD_STOP: 0x0000002C
                if (retVal == 0)
                {
                    Log.d(TAG, "SET_DEMOD_STOP ok\n");
                }

                // Read SET_DEMOD_STOP address value
                retVal = MainActivity.endeavour.Demod_Atsc1ReadRegisters(deviceId, 0x0000002C, regLen, setVal);     // SET_DEMOD_STOP: 0x0000002C
                if (retVal == 0)
                {
                    Log.d(TAG, "2. read SET_DEMOD_STOP: 0x" + Integer.toHexString(setVal[0] & 0xFF) + ", 0x" + Integer.toHexString(setVal[1] & 0xFF) + ", 0x" + Integer.toHexString(setVal[2] & 0xFF) + ", 0x" + Integer.toHexString(setVal[3] & 0xFF));

                }

                // Write SET_DEMOD_START address value
                // SET_DEMOD_START 0x00000048
                setVal[0] = (byte) 0x45;
                setVal[1] = (byte) 0x78;
                setVal[2] = (byte) 0x00;
                setVal[3] = (byte) 0x00;
                retVal = MainActivity.endeavour.Demod_Atsc1WriteRegisters(deviceId, 0x00000048, regLen, setVal);
                if (retVal == 0)
                {
                    Log.d(TAG, "SET_DEMOD_START ok\n");
                }

                // Read SET_DEMOD_START address value
                // SET_DEMOD_START 0x00000048
                setVal[0] = 0x0;
                retVal = MainActivity.endeavour.Demod_Atsc1ReadRegisters(deviceId, 0x00000048, regLen, setVal);
                if (retVal == 0)
                {
                    Log.d(TAG, "1. read SET_DEMOD_START: 0x" + Integer.toHexString(setVal[0] & 0xFF) + ", 0x" + Integer.toHexString(setVal[1] & 0xFF) + ", 0x" + Integer.toHexString(setVal[2] & 0xFF) + ", 0x" + Integer.toHexString(setVal[3] & 0xFF));
                }
*/
                thread.start();
            }
        });

        RadioButton deviceId0 = (RadioButton)view.findViewById(R.id.TsAnalysis_device_0);
        RadioButton deviceId1 = (RadioButton)view.findViewById(R.id.TsAnalysis_device_1);
        RadioButton deviceId2 = (RadioButton)view.findViewById(R.id.TsAnalysis_device_2);
        RadioButton deviceId3 = (RadioButton)view.findViewById(R.id.TsAnalysis_device_3);
        
        deviceId0.setChecked(true);

        if(MainActivity.channels == 1)
        {
            deviceId1.setEnabled(false);
            deviceId2.setEnabled(false);
            deviceId3.setEnabled(false);
        }

        return view;        
    }
    
    public void onResume()
    {
        Log.v(TAG, Debug.getLineInfo());


        if(already_GetTs == false) {
            already_GetTs = true;
            drop_data_time = 0;
            thread = new GetTSThread();
            thread.start();
        }

        get_cpu_status = true;
        thread = new GetCPUUsageThread();
        thread.start();
        super.onResume();
    }   
    
    @Override
    public void onStop()
    {
        Log.v(TAG, Debug.getLineInfo());
        
        already_GetTs = false;
        get_cpu_status = false;
        Message msg = new Message();
        msg.what = TS_STATUS_CLEAR;
        handler.sendMessage(msg);

        super.onStop();
    }

    public class TsStatus
    {
        public int PID;
        public long Packets_counter;
        public long packet_loss_times;
        public long Continuity_counter;
        public long packet_loss_num;
        public long packet_error_num;
        public long KBitsRate;
        public boolean PIDFilterLock;
        
        public TsStatus()
        {
            PID = -1;
            Packets_counter = 0;
            packet_loss_times = 0;
            Continuity_counter = -1;
            packet_loss_num = 0;
            packet_error_num = 0;
            KBitsRate = 0;
            PIDFilterLock = false;
        }
    }
    
    private static final String[] title = {
        "PID", "Packets Count", "Loss Count", "Error Count", "Kbps"};
    
    private static final String[] title_2 = {
        "PSB Buffer Status", "Packet Loss Count", "Total Kbps", "CPU"};

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
            text2.setTextColor(Color.RED);
 
            return convertView;
        }
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
  }
