package cn.freeeditor.sdk;

public class NativeMessage {

    private long nativeMessage;

    public NativeMessage(long nativeMessage){
        this.nativeMessage = nativeMessage;
    }

    public void release(){
        releaseMessage(nativeMessage);
    }

    public long getNativeMessage(){
        return nativeMessage;
    }

    public byte[] getBuffer(){
        if (nativeMessage == 0){
            return null;
        }
        return getBuffer(nativeMessage);
    }

    private native byte[] getBuffer(long nativeMessage);
    private native void releaseMessage(long nativeMessage);
}
