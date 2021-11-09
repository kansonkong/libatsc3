package org.ngbp.libatsc3.phy.views;

import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import org.ngbp.libatsc3.phy.models.RfScanResultModel;
import org.ngbp.libatsc3.sampleapp.R;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;


public class RfScanResultsListAdapter extends RecyclerView.Adapter<RfScanResultsListAdapter.MyViewHolder> {
    List<RfScanResultModel> mDataset;

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder
    public static class MyViewHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        public LinearLayout RfScanResultRow;
        public MyViewHolder(LinearLayout v) {
            super(v);
            RfScanResultRow = v;
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public RfScanResultsListAdapter(List<RfScanResultModel> myDataset) {
        mDataset = myDataset;
    }

    // Create new views (invoked by the layout manager)
    @Override
    public RfScanResultsListAdapter.MyViewHolder onCreateViewHolder(ViewGroup parent,
                                                                    int viewType) {
        // create a new view
        LinearLayout v = (LinearLayout) LayoutInflater.from(parent.getContext())
                .inflate(R.layout.rf_scan_results_row, parent, false);
//
        MyViewHolder vh = new MyViewHolder(v);
        return vh;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(MyViewHolder holder, int position) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        RfScanResultModel rfScanResultModel = mDataset.get(position);

        ((TextView) holder.RfScanResultRow.findViewById(R.id.txtFreqMhz)).setText(""+rfScanResultModel.frequency_mhz);
        ((TextView) holder.RfScanResultRow.findViewById(R.id.txtRssi)).setText(String.format("%d.%03d dB/%d.%03d dB", rfScanResultModel.min_rssi/1000, (-rfScanResultModel.min_rssi)%1000, rfScanResultModel.max_rssi/1000, (-rfScanResultModel.max_rssi)%1000));
        ((TextView) holder.RfScanResultRow.findViewById(R.id.txtSnr)).setText(String.format("%.2f/%.2f", (float)rfScanResultModel.min_snr1000/1000.0, (float)rfScanResultModel.max_snr1000/1000.0));
        ((TextView) holder.RfScanResultRow.findViewById(R.id.txtDemodLock)).setText(""+rfScanResultModel.tuner_lock);
        ((TextView) holder.RfScanResultRow.findViewById(R.id.txtRfStatLock)).setText(""+rfScanResultModel.demod_lock_status);
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}

