package cn.freeeditor.sdk;


public class JNIMessage {

    public final int key;
    public final long number;
    public final double decimal;
    public final Object obj;
    public final String string;

    public JNIMessage(){
        this.key = 0;
        this.number = 0;
        this.decimal = 0.0f;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key){
        this.key = key;
        this.number = 0;
        this.decimal = 0.0f;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key, long number){
        this.key = key;
        this.number = number;
        this.decimal = 0.0f;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key, double decimal){
        this.key = key;
        this.number = 0;
        this.decimal = decimal;
        this.obj = null;
        this.string = null;
    }

    public JNIMessage(int key, Object obj){
        this.key = key;
        this.number = 0;
        this.decimal = 0.0f;
        this.obj = obj;
        this.string = null;
    }

    public JNIMessage(int key, String json){
        this.key = key;
        this.number = 0;
        this.decimal = 0.0f;
        this.obj = null;
        this.string = json;
    }

    public JNIMessage(int key, long number, double decimal, Object obj, String string){
        this.key = key;
        this.number = number;
        this.decimal = decimal;
        this.obj = obj;
        this.string = string;
    }
}
