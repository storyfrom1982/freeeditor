package cn.freeeditor.sdk;


public class JNIMessage {

    public final int key;
    public long i64;
    public double f64;
    public Object obj;
    public String json;

    public JNIMessage(){
        key = 0;
        i64 = 0;
        f64 = 0.0;
        obj = null;
        json = null;
    }

    public JNIMessage(int key){
        this.key = key;
    }

    public JNIMessage(int key, long number){
        this.key = key;
        i64 = number;
    }

    public JNIMessage(int key, double number){
        this.key = key;
        f64 = number;
    }

    public JNIMessage(int key, String json){
        this.key = key;
        this.json = json;
    }

    public JNIMessage(int key, Object obj){
        this.key = key;
        this.obj = obj;
    }
}
