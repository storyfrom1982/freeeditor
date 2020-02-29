package cn.freeeditor.sdk;


public class JNIMessage {

    public final int key;
    public final long ptr;
    public final String string;

    public JNIMessage(){
        this.key = 0;
        this.ptr = 0;
        this.string = null;
    }

    public JNIMessage(int key){
        this.key = key;
        this.ptr = 0;
        this.string = null;
    }

    public JNIMessage(int key, long number){
        this.key = key;
        this.ptr = number;
        this.string = null;
    }

    public JNIMessage(int key, Object obj){
        this.key = key;
        this.ptr = getPointer(obj);
        this.string = null;
    }

    public JNIMessage(int key, String string){
        this.key = key;
        this.ptr = 0;
        this.string = string;
    }

    public JNIMessage(int key, long ptr, String string){
        this.key = key;
        this.ptr = ptr;
        this.string = string;
    }

    public Object getObject(){
        return getObject(ptr);
    }

    private native Object getObject(long ptr);
    private native long getPointer(Object obj);
}
