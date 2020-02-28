package cn.freeeditor.sdk;


public class JNIMessage {

    private static final int PktMsgType_Object = -1;
    private static final int PktMsgType_None = 0;
    private static final int PktMsgType_Ptr = 1;
    private static final int PktMsgType_String = 2;

    public final int key;
    public final int type;
    public final long ptr;
    public final Object obj;
    public final String string;

    public JNIMessage(){
        this.key = 0;
        this.type = PktMsgType_None;
        this.ptr = 0;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key){
        this.key = key;
        this.type = PktMsgType_None;
        this.ptr = 0;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key, long number){
        this.key = key;
        this.type = PktMsgType_Ptr;
        this.ptr = number;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key, Object obj){
        this.key = key;
        this.type = PktMsgType_Object;
        this.ptr = 0;
        this.obj = obj;
        this.string = null;
    }

    public JNIMessage(int key, String string){
        this.key = key;
        this.type = PktMsgType_String;
        this.ptr = 0;
        this.obj = null;
        this.string = string;
    }
}
