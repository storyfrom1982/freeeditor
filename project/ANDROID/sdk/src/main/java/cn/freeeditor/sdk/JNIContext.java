package cn.freeeditor.sdk;


abstract public class JNIContext {

    public JNIContext(){
        jniContext = createContext();
        setContextName(super.getClass().getName(), jniContext);
    }

    public void release(){
        deleteContext(jniContext);
        jniContext = 0;
    }

    public long getJniContext(){
        return jniContext;
    }

    public void connectContext(long messageContext){
        connectContext(messageContext, jniContext);
    }

    protected abstract void onPutMessage(JNIMessage msg);
    protected abstract JNIMessage onGetMessage(int key);

    protected long onGetLong(int key){
        return onGetMessage(key).i64;
    }

    protected String onGetJson(int key){
        return onGetMessage(key).json;
    }

    protected Object onGetObject(int key){
        return onGetMessage(key).obj;
    }

    protected void onPutMessage(int key){
        onPutMessage(new JNIMessage(key));
    }

    protected void onPutLong(int key, long number){
        onPutMessage(new JNIMessage(key, number));
    }

    protected void onPutJson(int key, String msg){
        onPutMessage(new JNIMessage(key, msg));
    }

    protected void onPutObject(int key, Object obj){
        onPutMessage(new JNIMessage(key, obj));
    }

    protected void putMessage(int key){
        putMessage(key, jniContext);
    }

    protected void putLong(int key, long number){
        putLong(key, number, jniContext);
    }

    protected void putJson(int key, String msg){
        putJson(key, msg, jniContext);
    }

    protected void putObject(int key, Object obj){
        putObject(key, obj, jniContext);
    }

    protected void putBuffer(int key, byte[] data, int size){
        putBuffer(key, data, size, jniContext);
    }

    protected long getLong(int key){
        return getLong(key, jniContext);
    }

    protected String getJson(int key){
        return getJson(key, jniContext);
    }

    protected Object getObject(int key){
        return getObject(key, jniContext);
    }


    static {
        System.loadLibrary("freeeditor");
    }

    private long jniContext;

    private native long createContext();
    private native void deleteContext(long jniContext);
    private native void setContextName(String name, long jniContext);
    private native void connectContext(long messageContext, long jniContext);

    private native void putMessage(int key, long jniContext);
    private native void putLong(int key, long number, long jniContext);
    private native void putJson(int key, String json, long jniContext);
    private native void putObject(int key, Object obj, long jniContext);
    private native void putBuffer(int key, byte[] data, int size, long jniContext);

    private native String getJson(int key, long jniContext);
    private native long getLong(int key, long jniContext);
    private native Object getObject(int key, long jniContext);
}
