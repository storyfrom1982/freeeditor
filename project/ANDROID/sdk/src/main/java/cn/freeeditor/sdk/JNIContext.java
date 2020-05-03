package cn.freeeditor.sdk;


import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public class JNIContext implements Runnable {

    protected static final String TAG = "JNIContext";

    public JNIContext(){
        contextPointer = createContext(super.getClass().getName());
    }

    public void release(){
        deleteContext(contextPointer);
        contextPointer = 0;
    }

    public long getContextPointer(){
        return contextPointer;
    }

    public void connectContext(long messageContext){
        connectContext(messageContext, contextPointer);
    }

    public void disconnectContext(){
        disconnectContext(contextPointer);
    }

    protected JNIMessage onRequestMessage(int key){
        return obtainMessage(0);
    }

    protected long onRequestMessage1(int key){
        return onRequestMessage(key).getNativeMessage();
    }

    protected void onRecvMessage(long msg){
        JNIMessage jmsg = new JNIMessage(msg);
        msgHandler.sendMessage(msgHandler.obtainMessage(0, jmsg));
    }

    protected void sendMessage(int key){
        sendMessage(key, 0, contextPointer);
    }

    protected void sendMessage(int key, int event){
        sendMessage(key, event, contextPointer);
    }

    protected void sendMessage(int key, long ptr){
        sendMessage(key, ptr, contextPointer);
    }

    protected void sendMessage(int key, Object obj){
        sendMessage(key, obj, contextPointer);
    }

    protected void sendMessage(int key, String json){
        sendMessage(key, json, contextPointer);
    }

    protected void sendMessage(int key, int event, String json){
        sendMessage(key, event, json, contextPointer);
    }

    protected void sendMessage(int key, byte[] buffer, int length){
        sendMessage(key, buffer, length, contextPointer);
    }

    protected JNIMessage requestMessage(int key){
        return new JNIMessage(requestMessage(key, contextPointer));
    }

    protected JNIMessage obtainMessage(int key){
        return new JNIMessage(obtainMessage(key, contextPointer));
    }


    static {
        System.loadLibrary("freeeditor");
    }

    private long contextPointer;

    private native long createContext(String name);
    private native void deleteContext(long contextPointer);
    private native void connectContext(long messageContext, long contextPointer);
    private native void disconnectContext(long contextPointer);

    private native void sendMessage(int key, int event, long contextPointer);
    private native void sendMessage(int key, long ptr, long contextPointer);
    private native void sendMessage(int key, Object obj, long contextPointer);
    private native void sendMessage(int key, String json, long contextPointer);
    private native void sendMessage(int key, int event, String json, long contextPointer);
    private native void sendMessage(int key, byte[] buffer, int length, long contextPointer);

    private native long requestMessage(int key, long contextPointer);

    private native long obtainMessage(int key, long contextPointer);


    private String mName;
    private Thread mThread;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    void onFinalRelease(){}

    void onMessageProcessor(JNIMessage msg){}

    void startHandler(String name){
        mName = name;
        if (!isRunning.get()){
            mThread = new Thread(this);
            mThread.start();
            synchronized (isRunning){
                if (!isRunning.get()){
                    try {
                        isRunning.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

    void stopHandler(){
        if (isRunning.compareAndSet(true, false)){
            msgHandler.sendEmptyMessage(-10000);
            try {
                mThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void run() {
        Log.dumpThread(TAG, mName, "thread start");
        Looper.prepare();
        msgHandler = new JNIContext.MessageHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
        msgHandler.release();
        Log.dumpThread(TAG, mName, "thread stop");
    }

    JNIContext.MessageHandler msgHandler;

    private static final class MessageHandler extends Handler {
        final WeakReference<JNIContext> weakReference;
        MessageHandler(JNIContext processor){
            weakReference = new WeakReference<>(processor);
        }
        public void release(){
            removeCallbacksAndMessages(null);
            weakReference.clear();
        }
        @Override
        public void handleMessage(Message msg) {
            JNIContext processor = weakReference.get();
            if (processor != null){
                JNIMessage jmsg = (JNIMessage)msg.obj;
                if (jmsg != null){
                    processor.onMessageProcessor(jmsg);
                    jmsg.release();
                }else if (msg.what == -10000){
                    Looper.myLooper().quit();
                    processor.onFinalRelease();
                }
            }
        }
    }
}
