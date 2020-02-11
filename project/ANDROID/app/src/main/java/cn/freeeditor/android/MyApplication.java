package cn.freeeditor.android;

import android.app.Application;

import cn.freeeditor.sdk.MediaContext;

public class MyApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        MediaContext.Instance().setAppContext(getApplicationContext());
//        MContext.Apply(getApplicationContext());
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        MediaContext.Instance().release();
//        MContext.Instance().remove();
    }
}
