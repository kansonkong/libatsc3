package org.ngbp.libatsc3.middleware.android.phy.interfaces;

import org.ngbp.libatsc3.middleware.android.phy.models.BwPhyStatistics;
import org.ngbp.libatsc3.middleware.android.phy.models.RfPhyStatistics;

public interface IAtsc3NdkPHYBridgeCallbacks {

    public void onPhyLogMessage(String message);

    //jjustman-2021-02-04 - for catastrophic PHY error reporting to application
    public void onPhyError(String message);

    public void pushRfPhyStatisticsUpdate(RfPhyStatistics rfPhyStatistics);
    public void pushBwPhyStatistics(BwPhyStatistics bwPhyStatistics);


}
