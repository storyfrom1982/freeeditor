package cn.freeeditor.sdk;

public class Log {
    static {
        System.loadLibrary("freeeditor");
    }
    public static native void d(String tag, String msg);
    public static native void e(String tag, String msg);
}
