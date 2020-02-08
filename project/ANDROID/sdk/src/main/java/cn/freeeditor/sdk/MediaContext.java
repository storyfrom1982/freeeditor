package cn.freeeditor.sdk;

import android.content.Context;

import java.lang.ref.WeakReference;

public class MediaContext extends JNIContext {

    public static final int Cmd_GetRecord = 1;
    public static final int Cmd_GetRecordConfig = 3;
    public static final int Cmd_GetVideoView = 5;

    private static final int Cmd_OnGet_Camera = 2;
    private static final int Cmd_OnGet_Microphone = 4;

    private static MediaContext jniContext = null;

    public static MediaContext Instance(){
        if (jniContext == null){
            jniContext = new MediaContext();
        }
        return jniContext;
    }

    private WeakReference<Context> mAppCtxReference = null;

    public void setApplicationContext(Context context){
        mAppCtxReference = new WeakReference<>(context);
    }

    public void release(){
        super.release();
        deleteContext(messageContext);
        messageContext = 0;
    }

    @Override
    public void onPutObject(int key, Object obj) {

    }

    @Override
    public void onPutMessage(int key, String msg) {

    }

    @Override
    public void onPutContext(int key, long ctx) {

    }

    @Override
    public Object onGetObject(int key) {
        return null;
    }

    @Override
    public String onGetMessage(int key) {
        return null;
    }

    public long newVideoViewContext(){
        return getContext(Cmd_GetVideoView);
    }

    @Override
    public long onGetContext(int key) {
        switch (key){
            case Cmd_OnGet_Camera:
                return createCamera();
            case Cmd_OnGet_Microphone:
                break;
            default:
                break;
        }
        return 0;
    }

    private MediaContext(){
        messageContext = createContext();
        setMessageContext(messageContext);
    }

    private long createCamera(){
        VideoCamera camera = new VideoCamera();
        return camera.getMessageContext();
    }

    private long messageContext;

    private native long createContext();

    public native void deleteContext(long messageContext);

    public native void debug();
}
