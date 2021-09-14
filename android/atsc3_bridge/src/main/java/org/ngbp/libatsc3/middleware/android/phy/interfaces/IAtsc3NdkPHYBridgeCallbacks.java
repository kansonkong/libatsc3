package org.ngbp.libatsc3.middleware.android.phy.interfaces;

import org.ngbp.libatsc3.middleware.android.phy.models.BwPhyStatistics;
import org.ngbp.libatsc3.middleware.android.phy.models.L1D_timePhyInformation;
import org.ngbp.libatsc3.middleware.android.phy.models.RfPhyStatistics;

public interface IAtsc3NdkPHYBridgeCallbacks {

    public void onPhyLogMessage(String message);

    //jjustman-2021-02-04 - for catastrophic PHY error reporting to application
    public void onPhyError(String message);

    public void pushRfPhyStatisticsUpdate(RfPhyStatistics rfPhyStatistics);
    public void pushBwPhyStatistics(BwPhyStatistics bwPhyStatistics);

    //jjustman-2021-08-31 - push L1d_time_* info for SFN validation against kronons NTP clock client
    public void pushL1d_TimeInfo(L1D_timePhyInformation l1dTimePhyInformation);


}
