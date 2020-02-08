package cn.freeeditor.sdk;

import java.nio.ByteBuffer;

public class JNIContext {


    public interface JNIListener {
        void onPutMessage(int key, String msg);
        void onPutObject(int key, Object obj);
        void onPutContext(int key, long ctx);
        Object onGetObject(int key);
        String onGetMessage(int key);
        long onGetContext(int key);
    }

    private JNIListener listener;

    public JNIContext(){
        mCtx = createContext();
    }

    public void setListener(JNIListener listener){
        this.listener = listener;
    }

    public void setContext(long messageContext){
        setContext(messageContext, mCtx);
    }

    public long getContext(){
        return mCtx;
    }

    public void release(){
        deleteContext(mCtx);
        mCtx = 0;
    }

    public void onPutObject(int key, Object obj) {
        listener.onPutObject(key, obj);
    }

    public void onPutMessage(int key, String msg){
        listener.onPutMessage(key, msg);
    }

    public void onPutContext(int key, long ctx){
        listener.onPutContext(key, ctx);
    }

    public Object onGetObject(int key){
        return listener.onGetObject(key);
    }

    public String onGetMessage(int key){
        return listener.onGetMessage(key);
    }

    public long onGetContext(int key){
        return listener.onGetContext(key);
    }

    public void putObject(int key, Object obj){
        putObject(key, obj, mCtx);
    }

    public void putMessage(int key, String msg){
        putMessage(key, msg, mCtx);
    }

    public void putContext(int key, long messageContext){
        putContext(key, messageContext, mCtx);
    }

    public void putData(int key, byte[] data, int size){
        putData(key, data, size, mCtx);
    }

    public long getContext(int key){
        return getContext(key, mCtx);
    }

    public String getMessage(int key){
        return getMessage(key, mCtx);
    }

    public Object getObject(int key){
        return getObject(key, mCtx);
    }

    private long mCtx;

    private native long createContext();
    private native void deleteContext(long mCtx);
    private native void setContext(long messageContext, long mCtx);

    private native void putObject(int key, Object obj, long mCtx);
    private native void putMessage(int key, String msg, long mCtx);
    private native void putContext(int key, long messageContext, long mCtx);
    private native void putData(int key, byte[] data, int size, long mCtx);
    private native long getContext(int key, long mCtx);
    private native String getMessage(int key, long mCtx);
    private native Object getObject(int key, long mCtx);
}
