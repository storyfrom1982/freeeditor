package cn.freeeditor.sdk;

import java.nio.ByteBuffer;

public class MediaRecord {


    private long jniObject;
    private final JNIHandler jniHandler;

    public MediaRecord(){
        jniHandler = new JNIHandler(new JNIHandler.IJNIListener() {
            @Override
            public int onPutObject(int type, long obj) {
                return 0;
            }

            @Override
            public int onPutMessage(int cmd, String msg) {
                Log.e("MediaRecord", msg);
                return 0;
            }

            @Override
            public int onPutData(byte[] data, int size) {
                return 0;
            }

            @Override
            public Object onGetObject(int type) {
                return null;
            }

            @Override
            public String onGetMessage(int cmd) {
                return null;
            }

            @Override
            public ByteBuffer onGetBuffer() {
                return null;
            }
        });

        jniObject = JNIContext.Instance().getObject(0);
        jniHandler.setContext(jniObject);
        jniHandler.putMessage(0,"12345");
    }

    public void release(){
        JNIContext.Instance().deleteObject(jniObject);
        jniHandler.release();
    }
}
