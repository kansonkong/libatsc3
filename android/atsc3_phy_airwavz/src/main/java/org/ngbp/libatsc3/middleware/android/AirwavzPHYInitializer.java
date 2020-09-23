package org.ngbp.libatsc3.middleware.android;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.startup.Initializer;

import org.ngbp.libatsc3.middleware.android.phy.AirwavzPHYAndroid;

import java.util.Collections;
import java.util.List;

public class AirwavzPHYInitializer implements Initializer<AirwavzPHYAndroid> {
    @NonNull
    @Override
    public AirwavzPHYAndroid create(@NonNull Context context) {
        return new AirwavzPHYAndroid();
    }

    @NonNull
    @Override
    public List<Class<? extends Initializer<?>>> dependencies() {
        return Collections.emptyList();
    }
}
