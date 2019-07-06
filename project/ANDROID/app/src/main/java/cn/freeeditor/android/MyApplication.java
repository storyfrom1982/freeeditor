package cn.freeeditor.android;

import android.app.Application;

import cn.freeeditor.sdk.MContext;

public class MyApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        MContext.Apply(getApplicationContext());
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        MContext.Instance().remove();
    }
}
