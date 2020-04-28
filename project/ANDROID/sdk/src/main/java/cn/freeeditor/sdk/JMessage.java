package cn.freeeditor.sdk;

public class JMessage {

    private final long nativeMessage;

    public JMessage(long nativeMessage){
        this.nativeMessage = nativeMessage;
    }

    public long getNativeMessage(){
        return nativeMessage;
    }

    public int getKey(){
        return getKey(nativeMessage);
    }

    public int getEvent(){
        return getEvent(nativeMessage);
    }

    public Object getObject(){
        return getObject(nativeMessage);
    }

    public String getString(){
        return getString(nativeMessage);
    }

    public byte[] getBuffer(){
        return getBuffer(nativeMessage);
    }

    public long getPtr(){
        return getPtr(nativeMessage);
    }

    private native int getKey(long nativeMessage);

    private native int getEvent(long nativeMessage);

    private native Object getObject(long nativeMessage);

    private native String getString(long nativeMessage);

    private native long getPtr(long nativeMessage);

    private native byte[] getBuffer(long nativeMessage);
}
