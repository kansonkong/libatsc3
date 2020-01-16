package org.ngbp.libatsc3.android;
//
//import com.google.android.things.contrib.driver.ht16k33.AlphanumericDisplay;
//import com.google.android.things.contrib.driver.ht16k33.Ht16k33;
//import com.google.android.things.contrib.driver.rainbowhat.RainbowHat;

import java.io.IOException;

public class ThingsUI {

   public static void WriteToAlphaDisplay(String toWrite) throws IOException {
//       try {
//            AlphanumericDisplay segment = RainbowHat.openDisplay();
//            segment.setBrightness(Ht16k33.HT16K33_BRIGHTNESS_MAX); //hack
//            segment.display(toWrite);
//            segment.setEnabled(true);
//            // Close the device when done.
//            segment.close();
//            //®AlphanumericDisplay
//        } catch (Exception | java.lang.NoClassDefFoundError ex) {
//           //re-throw so we can unwind our worker thread if we don't have this feature
//           throw ex;
//            // 2019-10-03 02:22:23.109 19905-19950/org.ngbp.libatsc3 D/main: Failed resolution of: Lcom/google/android/things/pio/PeripheralManager;
//            // Log.d(TAG, ex.getLocalizedMessage());f
//        }
    }


    public static void WriteToAlphaDisplayNoEx(String toWrite) {
//        try {
//            AlphanumericDisplay segment = RainbowHat.openDisplay();
//            segment.setBrightness(Ht16k33.HT16K33_BRIGHTNESS_MAX); //hack
//            segment.display(toWrite);
//            segment.setEnabled(true);
//            // Close the device when done.
//            segment.close();
//            //®AlphanumericDisplay
//        } catch (Exception | java.lang.NoClassDefFoundError ex) {
//            //re-throw so we can unwind our worker thread if we don't have this feature
//
//            // 2019-10-03 02:22:23.109 19905-19950/org.ngbp.libatsc3 D/main: Failed resolution of: Lcom/google/android/things/pio/PeripheralManager;
//            // Log.d(TAG, ex.getLocalizedMessage());f
//        }
    }
}
