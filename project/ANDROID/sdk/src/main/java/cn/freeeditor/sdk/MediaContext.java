package cn.freeeditor.sdk;

import android.content.Context;

import java.lang.ref.WeakReference;

public class MediaContext extends JNIContext {

    private static final int GetMsg_CrateRecorder = 1;
    private static final int GetMsg_GetRecorderConfig = 2;

    private static final int OnGetMsg_CreateCamera = 1;
    private static final int OnGetMsg_CreateMicrophone = 2;

    private static MediaContext jniContext = null;

    public static MediaContext Instance(){
        if (jniContext == null){
            jniContext = new MediaContext();
        }
        return jniContext;
    }

    private WeakReference<Context> mAppCtxReference = null;

    public void setAppContext(Context context){
        mAppCtxReference = new WeakReference<>(context);
    }

    public Context getAppContext(){
        return mAppCtxReference.get();
    }

    public void release(){
        super.release();
        deleteContext(mediaContext);
        mediaContext = 0;
    }

    public long createRecorder(){
        return getPointer(GetMsg_CrateRecorder);
    }

    public String getRecorderConfig(){
        return getJson(GetMsg_GetRecorderConfig);
    }

    @Override
    protected void onPutMessage(JNIMessage msg) {

    }

    @Override
    protected JNIMessage onGetMessage(int key) {
        switch (key){
            case OnGetMsg_CreateCamera:
                return new JNIMessage(key, createCamera());
            case OnGetMsg_CreateMicrophone:
                break;
            default:
                break;
        }
        return new JNIMessage(0);
    }

    private MediaContext(){
        mediaContext = createContext();
        connectContext(mediaContext);
    }

    private long createCamera(){
        VideoCamera camera = new VideoCamera();
        return camera.getJniContext();
    }

    private long mediaContext;

    private native long createContext();

    public native void deleteContext(long mediaContext);

    public native void debug();
}
