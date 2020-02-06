package cn.freeeditor.sdk;

import android.content.Context;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;

public class JNIContext extends JNIHandler {

    public static final int Cmd_GetRecord = 0;
    public static final int Cmd_GetRecordConfig = 1;

    private static final int Cmd_OnGet_Camera = 0;
    private static final int Cmd_OnGet_Microphone = 1;

    private static JNIContext jniContext = null;

    public static JNIContext Instance(){
        if (jniContext == null){
            jniContext = new JNIContext();
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

    private JNIContext(){
        setListener(new IJNIListener() {
            @Override
            public int onPutObject(int type, long obj) {
                return 0;
            }

            @Override
            public int onPutMessage(int cmd, String msg) {
                return 0;
            }

            @Override
            public int onPutData(byte[] data, int size) {
                return 0;
            }

            @Override
            public long onGetObject(int type) {
                switch (type){
                    case Cmd_OnGet_Camera:
                        return createCamera();
                    case Cmd_OnGet_Microphone:
                        break;
                    default:
                        break;
                }
                return 0;
            }

            @Override
            public String onGetMessage(int cmd) {
                return null;
            }

            @Override
            public ByteBuffer onGetBuffer() {
                return null;
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
