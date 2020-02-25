package cn.freeeditor.sdk;


import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public class VideoSurfaceView extends JNIContext implements SurfaceHolder.Callback {

    private SurfaceView surfaceView;

    private static final int PutMsg_SurfaceError = 0;
    private static final int PutMsg_SurfaceCreated = 1;
    private static final int PutMsg_SurfaceChanged = 2;
    private static final int PutMsg_SurfaceDestroyed = 3;

    private static final int OnPutMsg_Destroy = 0;
    private static final int OnPutMsg_RegisterCallback = 1;


//    private final Thread mMessageThread;
//    private final AtomicBoolean isRunning = new AtomicBoolean(false);


    public VideoSurfaceView() {
        startHandler();
//        mMessageThread = new Thread(this);
//        mMessageThread.start();
//        synchronized (isRunning){
//            if (!isRunning.get()){
//                try {
//                    isRunning.wait();
//                } catch (InterruptedException e) {
//                    e.printStackTrace();
//                }
//            }
//        }
    }

    public void release(){
        stopHandler();
//        mMessageHandler.sendMessage(mMessageHandler.obtainMessage(OnPutMsg_Destroy));
//        try {
//            mMessageThread.join();
//        } catch (InterruptedException e) {
//            e.printStackTrace();
//        }
    }

    public SurfaceView getSurfaceView(){
        return surfaceView;
    }

    public void setSurfaceView(SurfaceView view){
        surfaceView = view;
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return null;
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {
        msgHandler.sendMessage(msgHandler.obtainMessage(msg.key, msg));
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        sendMessage(PutMsg_SurfaceCreated, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        sendMessage(PutMsg_SurfaceChanged);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        sendMessage(PutMsg_SurfaceDestroyed);
    }

//    @Override
//    public void run() {
//        Looper.prepare();
//        mMessageHandler = new MessageHandler(this);
//        synchronized (isRunning){
//            isRunning.set(true);
//            isRunning.notifyAll();
//        }
//        Looper.loop();
//    }


    private void addSurfaceHolderCallback(){
        if (surfaceView != null) {
//            surfaceView.setVisibility(View.INVISIBLE);
            surfaceView.getHolder().addCallback(this);
//            surfaceView.setVisibility(View.VISIBLE);
        }
    }

//    private void destroy(){
//        Looper.myLooper().quit();
//        super.release();
//    }

    @Override
    void onFinalRelease() {
        super.release();
    }

    @Override
    void onMessageProcessor(Message msg) {
        switch (msg.what){
            case OnPutMsg_RegisterCallback:
                addSurfaceHolderCallback();
                break;
            default:
                break;
        }
    }

//    private MessageHandler mMessageHandler;
//
//    private static final class MessageHandler extends Handler {
//
//        final WeakReference<VideoSurfaceView> weakReference;
//
//        MessageHandler(VideoSurfaceView videoSurfaceView){
//            weakReference = new WeakReference<>(videoSurfaceView);
//        }
//
//        @Override
//        public void handleMessage(Message msg) {
//            VideoSurfaceView videoSurfaceView = weakReference.get();
//            if (videoSurfaceView != null){
//                switch (msg.what){
//                    case OnPutMsg_RegisterCallback:
//                        videoSurfaceView.addSurfaceHolderCallback();
//                        break;
//                    case OnPutMsg_Destroy:
//                        videoSurfaceView.destroy();
//                        break;
//                    default:
//                        break;
//                }
//            }
//        }
//    }
}
