package cn.freeeditor.sdk.jni;

public class Message {

    private long nativeMessage;

    public Message(long nativeMessage){
        this.nativeMessage = nativeMessage;
    }

    public void release(){
        if (nativeMessage != 0){
            release(nativeMessage);
            nativeMessage = 0;
        }
    }

    public void setInt(int i32){
        setInt(i32, nativeMessage);
    }

    public void setLong(long i64){
        setLong(i64, nativeMessage);
    }

    public void setFloat(float f32){
        setFloat(f32, nativeMessage);
    }

    public void setDouble(double f64){
        setDouble(f64, nativeMessage);
    }

    public void setData(byte[] buffer){
        setData(buffer, nativeMessage);
    }

    public void setObject(Object obj){
        setObject(obj, nativeMessage);
    }

    public void setString(String str){
        setString(str, nativeMessage);
    }

    public int getKey(){
        return getKey(nativeMessage);
    }

    public int getInt(){
        return getInt(nativeMessage);
    }

    public long getLong(){
        return getLong(nativeMessage);
    }

    public float getFloat(){
        return getFloat(nativeMessage);
    }

    public Double getDouble(){
        return getDouble(nativeMessage);
    }

    public byte[] getData(){
        return getData(nativeMessage);
    }

    public byte[] getBuffer(){
        return getBuffer(nativeMessage);
    }

    public Object getObject(){
        return getObject(nativeMessage);
    }

    public String getString(){
        return getString(nativeMessage);
    }

    public long getNativeMessage(){
        return nativeMessage;
    }


    private native int getKey(long nativeMessage);

    private native int getInt(long nativeMessage);

    private native long getLong(long nativeMessage);

    private native float getFloat(long nativeMessage);

    private native double getDouble(long nativeMessage);

    private native byte[] getData(long nativeMessage);

    private native byte[] getBuffer(long nativeMessage);

    private native Object getObject(long nativeMessage);

    public native String getString(long nativeMessage);

    private native void setInt(int i32, long nativeMessage);

    private native void setLong(long i64, long nativeMessage);

    private native void setFloat(float f32, long nativeMessage);

    private native void setDouble(double f64, long nativeMessage);

    private native void setData(byte[] buffer, long nativeMessage);

    private native void setObject(Object obj, long nativeMessage);

    private native void setString(String str, long nativeMessage);

    private native void release(long nativeMessage);
}
