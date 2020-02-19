package cn.freeeditor.sdk;


abstract public class JNIContext {

    public JNIContext(){
        contextPointer = createContext();
        setContextName(super.getClass().getName(), contextPointer);
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

    public void disconnectContext(long messageContext){

    }

    protected abstract JNIMessage onObtainMessage(int key);
    protected abstract void onReceiveMessage(JNIMessage msg);

    protected void sendMessage(int key){
        sendMessage(key, contextPointer);
    }

    protected void sendMessage(JNIMessage msg){
        sendMessage(msg, contextPointer);
    }

    protected void sendMessage(int key, byte[] buffer, int size){
        sendMessage(key, buffer, size, contextPointer);
    }

    protected JNIMessage getMessage(int key){
        return getMessage(key, contextPointer);
    }

    protected JNIMessage createJniMessage(int key, long number, double decimal, Object obj, String string){
        return new JNIMessage(key, number, decimal, obj, string);
    }

    static {
        System.loadLibrary("freeeditor");
    }

    private long contextPointer;

    private native long createContext();
    private native void deleteContext(long contextPointer);
    private native void setContextName(String name, long contextPointer);
    private native void connectContext(long messageContext, long contextPointer);
    private native void disconnectContext(long messageContext, long contextPointer);

    private native JNIMessage getMessage(int key, long contextPointer);
    private native void sendMessage(int key, long contextPointer);
    private native void sendMessage(int key, byte[] buffer, int size, long contextPointer);
    private native void sendMessage(JNIMessage msg, long contextPointer);
}
