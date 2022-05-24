package org.ngbp.libatsc3.middleware.android;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.startup.Initializer;

import org.ngbp.libatsc3.middleware.android.phy.TolkaPHYAndroid;

import java.util.Collections;
import java.util.List;
public class TolkaPHYInitializer implements Initializer<TolkaPHYAndroid> {

    @NonNull
    @Override
    public TolkaPHYAndroid create(@NonNull Context context) {
        return new TolkaPHYAndroid();
    }

    @NonNull
    @Override
    public List<Class<? extends Initializer<?>>> dependencies() {
        return Collections.emptyList();
    }
}