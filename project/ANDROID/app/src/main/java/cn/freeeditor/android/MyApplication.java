package cn.freeeditor.android;

import android.app.Application;

import cn.freeeditor.sdk.JNIContext;
import cn.freeeditor.sdk.MContext;

public class MyApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        JNIContext.Instance().setApplicationContext(getApplicationContext());
//        MContext.Apply(getApplicationContext());
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        JNIContext.Instance().release();
//        MContext.Instance().remove();
    }
}
