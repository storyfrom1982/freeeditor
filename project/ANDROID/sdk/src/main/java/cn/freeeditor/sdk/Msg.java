package cn.freeeditor.sdk;

public class Msg {

    private static final int MSG_TYPE_INTEGER = 0;
    private static final int MSG_TYPE_FLOAT = 1;
    private static final int MSG_TYPE_OBJECT = 2;
    private static final int MSG_TYPE_STRING = 3;

    public final int key;
    public final int type;
    public final long i64;
    public final double f64;
    public final Object obj;

    public Msg(){
        key = 0;
        type = 0;
        i64 = 0;
        f64 = 0.0f;
        obj = null;
    }

    public Msg(int key){
        this.key = key;
        type = 0;
        i64 = 0;
        f64 = 0.0f;
        obj = null;
    }

    public Msg(int key, long i64){
        this.key = key;
        this.type = MSG_TYPE_INTEGER;
        this.i64 = i64;
        f64 = 0.0f;
        obj = null;
    }

    public Msg(int key, double f64){
        this.key = key;
        this.type = MSG_TYPE_FLOAT;
        this.f64 = f64;
        i64 = 0;
        obj = null;
    }

    public Msg(int key, String s){
        this.key = key;
        type = MSG_TYPE_STRING;
        obj = s;
        i64 = 0;
        f64 = 0.0f;
    }

    public Msg(int key, Object obj){
        this.key = key;
        this.type = MSG_TYPE_OBJECT;
        this.obj = obj;
        i64 = 0;
        f64 = 0.0f;
    }

    public String msgToString(){
        return (String) obj;
    }

    public boolean isResultOK(){
        return key == 0;
    }

    public boolean isResultObject(){
        return key == 0 && obj != null;
    }

    public boolean isResultInstance(){
        return key == 0 && i64 != 0;
    }
}
