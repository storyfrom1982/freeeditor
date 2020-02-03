package cn.freeeditor.sdk;

import java.nio.ByteBuffer;

public class JNIHandler {


    public interface IJNIListener {
        int onPutObject(int type, long obj);
        int onPutMessage(int cmd, String msg);
        int onPutData(byte[] data, int size);
        Object onGetObject(int type);
        String onGetMessage(int cmd);
        ByteBuffer onGetBuffer();
    }

    private final IJNIListener listener;

    public JNIHandler(IJNIListener listener){
        this.listener = listener;
    }

    public void setContext(long ctx){
        mCtx = createContext(ctx);
    }

    public void release(){
        deleteContext(mCtx);
        mCtx = 0;
    }

    public int onPutObject(int type, long obj) {
        return listener.onPutObject(type, obj);
    }

    public int onPutMessage(int cmd, String msg){
        return listener.onPutMessage(cmd, msg);
    }

    public int onPutData(byte[] data, int size){
        return listener.onPutData(data, size);
    }

    public Object onGetObject(int type){
        return listener.onGetObject(type);
    }

    public String onGetMessage(int cmd){
        return listener.onGetMessage(cmd);
    }

    public ByteBuffer onGetBuffer(){
        return listener.onGetBuffer();
    }

    public int putObject(int type, Object obj){
        return putObject(type, obj, mCtx);
    }

    public int putMessage(int cmd, String msg){
        return putMessage(cmd, msg, mCtx);
    }

    public int putData(byte[] data, int size){
        return putData(data, size, mCtx);
    }

    public long getObject(int type){
        return getObject(type, mCtx);
    }

    public String getMessage(int cmd){
        return getMessage(cmd, mCtx);
    }

    public ByteBuffer getBuffer(){
        return getBuffer(mCtx);
    }

    private long mCtx;

    private native long createContext(long ctx);
    private native void deleteContext(long mCtx);

    private native int putObject(int type, Object obg, long mCtx);
    private native int putMessage(int cmd, String msg, long mCtx);
    private native int putData(byte[] data, int size, long mCtx);
    private native long getObject(int type, long mCtx);
    private native String getMessage(int cmd, long mCtx);
    private native ByteBuffer getBuffer(long mCtx);
}
