package cn.freeeditor.sdk;


abstract public class JNIContext extends MessageProcessor {

    public JNIContext(){
        contextPointer = createContext(super.getClass().getName());
    }

    public void release(){
        deleteContext(contextPointer);
        contextPointer = 0;
    }

    public long getContextPointer(){
        return contextPointer;
    }

    public void connectContext(long messageContext){
        connectContext(messageContext, contextPointer);
    }

    public void disconnectContext(){
        disconnectContext(contextPointer);
    }

    protected abstract JNIMessage onObtainMessage(int key);
    protected abstract void onRecvMessage(JNIMessage msg);

    protected void sendMessage(int key){
        sendMessage(key, 0, contextPointer);
    }

    protected void sendMessage(int key, long ptr){
        sendMessage(key, ptr, contextPointer);
    }

    protected void sendMessage(int key, Object obj){
        sendMessage(key, obj, contextPointer);
    }

    protected void sendMessage(int key, String json){
        sendMessage(key, json, contextPointer);
    }

    protected void sendMessage(int key, byte[] buffer, long timestamp){
        sendMessage(key, buffer, timestamp, contextPointer);
    }

    protected JNIMessage requestMessage(int key){
        return requestMessage(key, contextPointer);
    }


    static {
        System.loadLibrary("freeeditor");
    }

    private long contextPointer;

    private native long createContext(String name);
    private native void deleteContext(long contextPointer);
    private native void connectContext(long messageContext, long contextPointer);
    private native void disconnectContext(long contextPointer);

    private native JNIMessage requestMessage(int key, long contextPointer);
//    private native void sendMessage(int key, long contextPointer);
    private native void sendMessage(int key, Object obj, long contextPointer);
    private native void sendMessage(int key, String json, long contextPointer);
    private native void sendMessage(int key, long ptr, long contextPointer);
    private native void sendMessage(int key, byte[] buffer, long timestamp, long contextPointer);
//    private native void sendMessage(JNIMessage msg, long contextPointer);
}
