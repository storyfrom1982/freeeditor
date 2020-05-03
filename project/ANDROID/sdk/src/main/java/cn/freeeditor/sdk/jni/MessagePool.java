package cn.freeeditor.sdk.jni;

public class MessagePool {

    public MessagePool(int bufferSize, int bufferCount, boolean isBlocking, String name){
        nativeHandler = createNativeHandler(bufferSize, bufferCount, isBlocking, name);
    }

    public void release(){
        if (nativeHandler != 0){
            releaseNativeHandler(nativeHandler);
            nativeHandler = 0;
        }
    }

    public Message obtainBufferMessage(int key){
        return new Message(newBufferMessage(key, nativeHandler));
    }

    private long nativeHandler;

    private native long createNativeHandler(int messageSize, int messageCount, boolean isBlocking, String name);

    private native void releaseNativeHandler(long nativeHandler);

    private native long newBufferMessage(int key, long nativeHandler);
}
