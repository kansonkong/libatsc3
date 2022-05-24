package com.api;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;

public class Debug
{
	
/*	public static int line()
	{
		return Thread.currentThread().getStackTrace()[2].getLineNumber();
	}
	
	public static String filename()
	{
		return Thread.currentThread().getStackTrace()[2].getFileName();
	}*/
	
    public static String getLineInfo()  
    {  
        StackTraceElement ste = new Throwable().getStackTrace()[1];  
        return ste.getFileName() + " " + ste.getMethodName() + ": Line " + ste.getLineNumber();  
    }
    
    public static void showmessage(Context context, long errorcode)
    {
    	AlertDialog.Builder builder = new AlertDialog.Builder(context);
    	String str = "Error code = 0x" + Long.toHexString(errorcode);
    	builder.setMessage(str)
    	       .setCancelable(false)
    	       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
    	           public void onClick(DialogInterface dialog, int id) {
    	           }
    	       });
    	AlertDialog alert = builder.create();
    	alert.show();
    }
    
    public static void showmessage(Context context, String str)
    {
    	AlertDialog.Builder builder = new AlertDialog.Builder(context);
    	builder.setMessage(str)
    	       .setCancelable(false)
    	       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
    	           public void onClick(DialogInterface dialog, int id) {
    	           }
    	       });
    	AlertDialog alert = builder.create();
    	alert.show();   	
    }
    
    public static void showmessage(Context context, String msg, long errorcode)
    {
    	AlertDialog.Builder builder = new AlertDialog.Builder(context);
    	String str = msg + ", Error code = 0x" + Long.toHexString(errorcode);
    	builder.setMessage(str)
    	       .setCancelable(false)
    	       .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
    	           public void onClick(DialogInterface dialog, int id) {
    	           }
    	       });
    	AlertDialog alert = builder.create();
    	alert.show();
    }    
}