package cn.freeeditor.sdk;

public class JNIMessagePool {

    public JNIMessagePool(int bufferSize, int bufferCount){
        mPool = createMessagePool(bufferSize, bufferCount);
    }

    public void release(){
        if (mPool != 0){
            releaseMessagePool(mPool);
            mPool = 0;
        }
    }

    public JMessage newMessage(int key){
        return new JMessage(allocMessage(key, mPool));
    }

    public JMessage newMessage(int key, int event){
        return new JMessage(allocMessage(key, event, mPool));
    }

    public JMessage newMessage(int key, long ptr){
        return new JMessage(allocMessage(key, ptr, mPool));
    }

    public JMessage newMessage(int key, Object obj){
        return new JMessage(allocMessage(key, obj, mPool));
    }

    public JMessage newMessage(int key, String str){
        return new JMessage(allocMessage(key, str, mPool));
    }

    public JMessage newMessage(int key, int event, long ptr, Object obj, String str) {
        return new JMessage(allocMessage(key, event, ptr, obj, str, mPool));
    }

    public void recycelMessage(JMessage msg){
        recycleMessage(msg.getNativeMessage(), mPool);
    }

    private long mPool;

    private native long createMessagePool(int bufferSize, int bufferCount);

    private native void releaseMessagePool(long mPool);

    private native long allocMessage(int key, long mPool);

    private native long allocMessage(int key, int event, long mPool);

    private native long allocMessage(int key, long ptr, long mPool);

    private native long allocMessage(int key, Object obj, long mPool);

    private native long allocMessage(int key, String str, long mPool);

    private native long allocMessage(int key, int event, long ptr, Object obj, String str, long mPool);

    private native void recycleMessage(long message, long mPool);
}
