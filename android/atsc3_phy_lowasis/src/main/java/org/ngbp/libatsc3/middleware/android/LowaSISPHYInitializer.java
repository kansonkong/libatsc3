package org.ngbp.libatsc3.middleware.android;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.startup.Initializer;

import org.ngbp.libatsc3.middleware.android.phy.LowaSISPHYAndroid;

import java.util.Collections;
import java.util.List;

public class LowaSISPHYInitializer implements Initializer<LowaSISPHYAndroid> {
    @NonNull
    @Override
    public LowaSISPHYAndroid create(@NonNull Context context) {
        return new LowaSISPHYAndroid();
    }

    @NonNull
    @Override
    public List<Class<? extends Initializer<?>>> dependencies() {
        return Collections.emptyList();
    }
}
