package cn.freeeditor.sdk;


abstract public class JNIContext {

    public JNIContext(){
        jniContext = createContext();
    }

    public void release(){
        deleteContext(jniContext);
        jniContext = 0;
    }

    public void setMessageContext(long messageContext){
        setMessageContext(messageContext, jniContext);
    }

    public long getMessageContext(){
        return jniContext;
    }

    protected abstract void onPutObject(int key, Object obj);

    protected abstract void onPutMessage(int key, String msg);

    protected abstract void onPutContext(int key, long ctx);

    protected abstract Object onGetObject(int key);

    protected abstract String onGetMessage(int key);

    protected abstract long onGetContext(int key);


    public void putObject(int key, Object obj){
        putObject(key, obj, jniContext);
    }

    public void putMessage(int key, String msg){
        putMessage(key, msg, jniContext);
    }

    public void putContext(int key, long messageContext){
        putContext(key, messageContext, jniContext);
    }

    public void putData(int key, byte[] data, int size){
        putData(key, data, size, jniContext);
    }

    public long getContext(int key){
        return getContext(key, jniContext);
    }

    public String getMessage(int key){
        return getMessage(key, jniContext);
    }

    public Object getObject(int key){
        return getObject(key, jniContext);
    }


    static {
        System.loadLibrary("freeeditor");
    }

    private long jniContext;

    private native long createContext();
    private native void deleteContext(long jniContext);
    private native void setMessageContext(long messageContext, long jniContext);

    private native void putObject(int key, Object obj, long jniContext);
    private native void putMessage(int key, String msg, long jniContext);
    private native void putContext(int key, long messageContext, long jniContext);
    private native void putData(int key, byte[] data, int size, long jniContext);
    private native long getContext(int key, long jniContext);
    private native String getMessage(int key, long jniContext);
    private native Object getObject(int key, long jniContext);
}
