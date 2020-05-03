package cn.freeeditor.sdk.jni;

import android.os.Handler;
import android.os.Looper;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

import cn.freeeditor.sdk.Log;

public abstract class MessageContext implements Runnable {

    protected static final String TAG = "MessageContext";

    private String mName;
    private Thread mThread;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    public MessageContext() {
        nativeHandler = createNativeHandler(super.getClass().getName());
    }

    public void release(){
        if (nativeHandler != 0){
            releaseNativeHandler(nativeHandler);
            nativeHandler = 0;
        }
    }


    MessageContext.MessageHandler msgHandler;

    void onFinalRelease(){}

    void onMessageProcessor(Message msg){}

    protected abstract Message onMessageRequest(int key);

    protected void startHandler(String name){
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

    protected void stopHandler(){
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
        msgHandler = new MessageContext.MessageHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
        msgHandler.release();
        Log.dumpThread(TAG, mName, "thread stop");
    }

    protected static final class MessageHandler extends Handler {
        final WeakReference<MessageContext> weakReference;
        MessageHandler(MessageContext processor){
            weakReference = new WeakReference<>(processor);
        }
        public void release(){
            removeCallbacksAndMessages(null);
            weakReference.clear();
        }
        @Override
        public void handleMessage(android.os.Message msg) {
            MessageContext processor = weakReference.get();
            if (processor != null){
                if (msg.what == -10000){
                    Looper.myLooper().quit();
                    processor.onFinalRelease();
                }else {
                    Message message = (Message) msg.obj;
                    processor.onMessageProcessor(message);
                    message.release();
                }
            }
        }
    }


    public long getNativeHandler(){
        return nativeHandler;
    }

    public void connectNativeContext(long nativeContext){
        connectContext(nativeContext, nativeHandler);
    }

    public void disconnectNativeContext(){
        disconnectContext(nativeHandler);
    }

    long onRequestMessage(int key){
        return onMessageRequest(key).getNativeMessage();
    }

    public Message obtainMessage(int key){
        return new Message(newMessage(key, nativeHandler));
    }

    protected void onRecvMessage(long nativeMessage){
        msgHandler.sendMessage(msgHandler.obtainMessage(0, new Message(nativeMessage)));
    }

    protected void sendMessage(Message msg){
        sendMessage(msg.getNativeMessage(), nativeHandler);
    }

    protected Message requestMessage(int key){
        return new Message(requestMessage(key, nativeHandler));
    }

    private long nativeHandler;

    private native long createNativeHandler(String name);
    private native void releaseNativeHandler(long nativeHandler);

    private native void connectContext(long nativeContext, long nativeHandler);
    private native void disconnectContext(long nativeHandler);

    private native void sendMessage(long msg, long nativeHandler);
    private native long requestMessage(int key, long nativeHandler);

    private native long newMessage(int key, long nativeHandler);
}
