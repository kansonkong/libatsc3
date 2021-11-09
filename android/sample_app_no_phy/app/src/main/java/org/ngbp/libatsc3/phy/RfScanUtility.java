package org.ngbp.libatsc3.phy;

import android.widget.EditText;

import org.ngbp.libatsc3.ServiceHandler;
import org.ngbp.libatsc3.phy.models.RfScanResultModel;
import org.ngbp.libatsc3.phy.views.RfScanResultsListAdapter;
import org.ngbp.libatsc3.sampleapp.MainActivity;
import org.ngbp.libatsc3.sampleapp.R;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

public class RfScanUtility implements Runnable{

    private EditText mRfScanStartFreqVal;
    private EditText mRfScanEndFreqVal;
    private EditText mRfScanLingerVal;

    public Boolean shouldRun = true;
    public Boolean isRunning = false;
    private RecyclerView mRfScanResultsView;
    private LinearLayoutManager layoutManager;
    private RfScanResultsListAdapter mRfScanResultsAdapter;
    private List<RfScanResultModel> myRfScanResultsDataset = Collections.synchronizedList(new ArrayList<RfScanResultModel>());

    public RfScanUtility(AppCompatActivity appCompatActivity) {

        //draw our RF scan results view
        mRfScanResultsView = (RecyclerView) appCompatActivity.findViewById(R.id.RfScanResultsView);

        // use this setting to improve performance if you know that changes
        // in content do not change the layout size of the RecyclerView
        //recyclerView.setHasFixedSize(true);

        // use a linear layout manager
        layoutManager = new LinearLayoutManager(appCompatActivity);
        mRfScanResultsView.setLayoutManager(layoutManager);

        mRfScanResultsAdapter = new RfScanResultsListAdapter(myRfScanResultsDataset);
        mRfScanResultsView.setAdapter(mRfScanResultsAdapter);

        mRfScanStartFreqVal = (EditText)appCompatActivity.findViewById(R.id.RfScanStartFreqVal);
        mRfScanEndFreqVal = (EditText)appCompatActivity.findViewById(R.id.RfScanEndFreqVal);
        mRfScanLingerVal = (EditText)appCompatActivity.findViewById(R.id.RfScanLingerDelayVal);

    }

    public void setEnabled(Boolean isEnabled) {
        mRfScanStartFreqVal.setEnabled(isEnabled);
        mRfScanEndFreqVal.setEnabled(isEnabled);
        mRfScanLingerVal.setEnabled(isEnabled);
    }

    @Override
    public void run() {
        int startFreq = Integer.parseInt(mRfScanStartFreqVal.getText().toString());
        int endFreq = Integer.parseInt(mRfScanEndFreqVal.getText().toString());
        int lingerClampedMS = Math.min(60000, Math.max(10, Integer.parseInt(mRfScanLingerVal.getText().toString())));
        while(shouldRun) {
            try {
                for(int freqMHz = startFreq; freqMHz <= endFreq && shouldRun; freqMHz+=6) {
                    //showMsgFromNative(String.format("Tuning to: %d", freqMHz));
   //                 MainActivity.Atsc3NdkApplicationBridge.ApiTune((freqMHz)*1000, 0);
                    RfScanResultModel rfScanResultModel = new RfScanResultModel();
                    rfScanResultModel.scan_start_ms = System.currentTimeMillis();
                    rfScanResultModel.frequency_mhz = freqMHz;

                    int loopCount = (lingerClampedMS/500);
                    int loopDelayMs = lingerClampedMS / loopCount;

                    //poll every 500ms for an updated status
                    for(int i=0; i <= (lingerClampedMS/500); i++) {
                        if( MainActivity.lastRfPhyStatistics != null) {
                            if ( MainActivity.lastRfPhyStatistics.rssi > rfScanResultModel.max_rssi) {
                                rfScanResultModel.max_rssi =  MainActivity.lastRfPhyStatistics.rssi;
                            }
                            if( MainActivity.lastRfPhyStatistics.rssi < rfScanResultModel.min_rssi) {
                                rfScanResultModel.min_rssi =  MainActivity.lastRfPhyStatistics.rssi;
                            }
                            if ( MainActivity.lastRfPhyStatistics.nSnr1000 > rfScanResultModel.max_snr1000) {
                                rfScanResultModel.max_snr1000 =  MainActivity.lastRfPhyStatistics.nSnr1000;
                            }
                            if ( MainActivity.lastRfPhyStatistics.nSnr1000 < rfScanResultModel.min_snr1000) {
                                rfScanResultModel.min_snr1000 =  MainActivity.lastRfPhyStatistics.nSnr1000;
                            }
                            if ( MainActivity.lastRfPhyStatistics.tuner_lock != 0) {
                                rfScanResultModel.tuner_lock =  MainActivity.lastRfPhyStatistics.tuner_lock;
                            }
                            if ( MainActivity.lastRfPhyStatistics.demod_lock_status != 0) {
                                rfScanResultModel.demod_lock_status =  MainActivity.lastRfPhyStatistics.demod_lock_status;
                            }
                        }
                        Thread.sleep(loopDelayMs);
                    }

                    rfScanResultModel.scan_end_ms = System.currentTimeMillis();
                    myRfScanResultsDataset.add(rfScanResultModel);

                    ServiceHandler.GetInstance().post(new Runnable() {
                        @Override
                        public void run() {
                            mRfScanResultsAdapter.notifyDataSetChanged();
                            mRfScanResultsView.smoothScrollToPosition(mRfScanResultsAdapter.getItemCount()-1);
                        }
                    });

                }

            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {

            }
        }
        isRunning = false;
    }
}

