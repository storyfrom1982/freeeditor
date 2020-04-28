package cn.freeeditor.sdk;

abstract public class JNIMessageContext extends JNIMessagePool {

    public JNIMessageContext() {
        super(10240, 10);
    }

    protected abstract JMessage onRequestMessage(int key);
    protected abstract void onRecvMessage(JMessage msg);


    private long contextPointer;

    private native long createContext(String name);
    private native void deleteContext(long contextPointer);
    private native void connectContext(long messageContext, long contextPointer);
    private native void disconnectContext(long contextPointer);

    private native void sendMessage(JMessage msg, long contextPointer);
    private native JMessage requestMessage(long contextPointer);
}
