package org.ngbp.libatsc3.middleware.android;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.startup.Initializer;

import org.ngbp.libatsc3.middleware.android.phy.SaankhyaPHYAndroid;

import java.util.Collections;
import java.util.List;

public class SaankhyaPHYInitializer implements Initializer<SaankhyaPHYAndroid> {
    @NonNull
    @Override
    public SaankhyaPHYAndroid create(@NonNull Context context) {
        return new SaankhyaPHYAndroid();
    }

    @NonNull
    @Override
    public List<Class<? extends Initializer<?>>> dependencies() {
        return Collections.emptyList();
    }
}
