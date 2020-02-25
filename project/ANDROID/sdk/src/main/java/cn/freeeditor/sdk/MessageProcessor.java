package cn.freeeditor.sdk;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public class MessageProcessor implements Runnable {

    private Thread mThread;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    void onFinalRelease(){}

    void onMessageProcessor(Message msg){}

    private void processMessage(Message msg){
        if (isRunning.get()){
            onMessageProcessor(msg);
        }else {
            Looper.myLooper().quit();
            onFinalRelease();
        }
    }

    void startHandler(){
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
            msgHandler.sendEmptyMessage(0);
            try {
                mThread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void run() {
        Looper.prepare();
        msgHandler = new MessageHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
    }

    MessageHandler msgHandler;

    protected static final class MessageHandler extends Handler {
        final WeakReference<MessageProcessor> weakReference;
        MessageHandler(MessageProcessor processor){
            weakReference = new WeakReference<>(processor);
        }
        @Override
        public void handleMessage(Message msg) {
            MessageProcessor processor = weakReference.get();
            if (processor != null){
                processor.processMessage(msg);
            }
        }
    }

}
