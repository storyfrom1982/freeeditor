package cn.freeeditor.sdk;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public class MessageProcessor implements Runnable {

    protected static final String TAG = "MessageProcessor";

    private String mName;
    private Thread mThread;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    void onFinalRelease(){}

    void onMessageProcessor(Message msg){}

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
        msgHandler = new MessageHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
        msgHandler.release();
        Log.dumpThread(TAG, mName, "thread stop");
    }

    MessageHandler msgHandler;

    protected static final class MessageHandler extends Handler {
        final WeakReference<MessageProcessor> weakReference;
        MessageHandler(MessageProcessor processor){
            weakReference = new WeakReference<>(processor);
        }
        public void release(){
            weakReference.clear();
        }
        @Override
        public void handleMessage(Message msg) {
            MessageProcessor processor = weakReference.get();
            if (processor != null){
                if (msg.what == -10000){
                    Looper.myLooper().quit();
                    processor.onFinalRelease();
                }
                processor.onMessageProcessor(msg);
            }
        }
    }

}
