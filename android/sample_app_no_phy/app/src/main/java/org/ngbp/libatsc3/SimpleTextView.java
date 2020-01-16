package org.ngbp.libatsc3;

import android.content.Context;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.DisplayMetrics;
import android.util.TypedValue;
import android.view.View;

public class SimpleTextView extends View {

    private static final int BUFFER_SIZE = 128;
    private int textSize = 8;
    private char[] buffer = { 0 };
    //new char[BUFFER_SIZE];
    private Paint paint = new Paint();
    private int padding = 2;
    private int LINE_HEIGHT = 16;

    public SimpleTextView(Context context) {
        super(context);
        this.initPaint();
    }

    public SimpleTextView(Context context, AttributeSet attrs) {
        super(context, attrs);

        // Get back some values from xml data
        this.padding = attrs.getAttributeIntValue("http://schemas.android.com/apk/res/android", "padding", this.padding);
        this.textSize = attrs.getAttributeIntValue("http://schemas.android.com/apk/res/android", "textSize", this.textSize);
        this.initPaint();
        //this.setLayerType(View.LAYER_TYPE_HARDWARE, null);
    }

    public void initPaint() {

        // Setting the paint
        this.padding = getPixels(TypedValue.COMPLEX_UNIT_DIP, this.padding);
        this.textSize = getPixels(TypedValue.COMPLEX_UNIT_SP, this.textSize);

        //this.paint.setAntiAlias(true);
        this.paint.setColor(Color.WHITE);
        this.paint.setTextSize(this.textSize);
    }

    @Override
    public void onDraw(Canvas canvas) {

        int start = 0, line = 0;
        // Display multi-lines content
        for (int i = 0; i < buffer.length; i++)
        {
            if (buffer[i] == '\n' || buffer[i] == '\0' || i == buffer.length-1) {
                canvas.drawText(buffer, start, i-start, this.padding, line++ * this.textSize * LINE_HEIGHT + this.padding, paint);
                start = i+1;
            }
        }
        //invalidate();
    }
    public void setText(String source) {
       // Log.d("SimpleTextView","msg is:" +source);

        buffer = source.toCharArray();

        //invalidate();
        //postInvalidate();
    }
    public void setText(StringBuilder source) {
        source.getChars(0, source.length(), buffer, 0);
    }

    private int getPixels(int unit, float size) {
        DisplayMetrics metrics = Resources.getSystem().getDisplayMetrics();
        return (int)TypedValue.applyDimension(unit, size, metrics);
    }
}

