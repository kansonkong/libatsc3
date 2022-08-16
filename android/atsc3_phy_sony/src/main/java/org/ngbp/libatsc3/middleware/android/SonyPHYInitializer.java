package org.ngbp.libatsc3.middleware.android;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.startup.Initializer;

import org.ngbp.libatsc3.middleware.android.phy.SonyPHYAndroid;

import java.util.Collections;
import java.util.List;

public class SonyPHYInitializer implements Initializer<SonyPHYAndroid> {

    @NonNull
    @Override
    public SonyPHYAndroid create(@NonNull Context context) {
        return new SonyPHYAndroid();
    }

    @NonNull
    @Override
    public List<Class<? extends Initializer<?>>> dependencies() {
        return Collections.emptyList();
    }
}