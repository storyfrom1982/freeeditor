package cn.freeeditor.sdk;

public class NativeMessagePool {

    private long nativeHandler;

    public NativeMessagePool(String name, int messageCount, int maxMessageCount, int messageSize){
        nativeHandler = createNativeHandler(name, messageCount, maxMessageCount, messageSize);
    }

    public void release(){
        releaseNativeHandler(nativeHandler);
    }

    public NativeMessage newNativeMessage(int key){
        return new NativeMessage(newNativeMessage(key, nativeHandler));
    }

    private native long createNativeHandler(String name, int messageCount, int maxMessageCount, int messageSize);
    private native void releaseNativeHandler(long nativeHandler);
    private native long newNativeMessage(int key, long nativeHandler);
}
