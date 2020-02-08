package cn.freeeditor.sdk;

import android.content.Context;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

public class MediaContext extends JNIContext {

    public static final int Cmd_GetRecord = 1;
    public static final int Cmd_GetRecordConfig = 3;

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
        deleteContext(mCtx);
        mCtx = 0;
    }

    private MediaContext(){
        setListener(new JNIListener() {
            @Override
            public void onPutMessage(int key, String msg) {

            }

            @Override
            public void onPutObject(int key, Object obj) {

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
        });
        mCtx = createContext();
        setContext(mCtx);
    }

    private long createCamera(){
        VideoCamera camera = new VideoCamera();
        return camera.getContext();
    }


    static {
        System.loadLibrary("freeeditor");
    }

    private long mCtx = 0;

    private native long createContext();

    private native void deleteContext(long mCtx);

    public native void debug();

    public native void deleteObject(long jniObject);
}
