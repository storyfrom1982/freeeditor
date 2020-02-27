package cn.freeeditor.sdk;


import android.os.Message;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.alibaba.fastjson.JSONObject;


public class VideoSurfaceView extends JNIContext implements SurfaceHolder.Callback {


    private static final int SendMsg_SurfaceCreated = 1;
    private static final int SendMsg_SurfaceChanged = 2;
    private static final int SendMsg_SurfaceDestroyed = 3;

    private static final int OnRecvMsg_WindowCreated = 1;
    private static final int OnRecvMsg_WindowDestroyed = 2;


    private SurfaceView surfaceView;
    private boolean isSurfaceCreated = false;
    private boolean isWindowCreated = false;
    private final Object lock = new Object();


    public VideoSurfaceView() {
        startHandler();
    }

    public void release(){
        stopHandler();
    }

    public SurfaceView getSurfaceView(){
        return surfaceView;
    }

    public void setSurfaceView(SurfaceView view){
        surfaceView = view;
        surfaceView.getHolder().addCallback(this);
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return new JNIMessage();
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {
        msgHandler.sendMessage(msgHandler.obtainMessage(msg.key, msg));
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        synchronized (lock){
            isSurfaceCreated = true;
            if (isWindowCreated){
                sendMessage(SendMsg_SurfaceCreated, holder.getSurface());
            }
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        synchronized (lock){
            if (isWindowCreated){
                JSONObject js = new JSONObject();
                js.put("width", width);
                js.put("height", height);
                sendMessage(SendMsg_SurfaceChanged, js.toJSONString());
            }
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        synchronized (lock){
            isSurfaceCreated = false;
            if (isWindowCreated){
                sendMessage(SendMsg_SurfaceDestroyed);
            }
        }
    }

    private void onWindowCreated(){
        synchronized (lock){
            isWindowCreated = true;
            if (isSurfaceCreated){
                sendMessage(SendMsg_SurfaceCreated, surfaceView.getHolder().getSurface());
                JSONObject js = new JSONObject();
                js.put("width", surfaceView.getWidth());
                js.put("height", surfaceView.getHeight());
                sendMessage(SendMsg_SurfaceChanged, js.toJSONString());
            }
        }
    }

    private void onWindowDestroyed(){
        synchronized (lock){
            isWindowCreated = false;
            if (isSurfaceCreated){
                sendMessage(SendMsg_SurfaceDestroyed);
            }
        }
    }

    @Override
    void onFinalRelease() {
        super.release();
    }

    @Override
    void onMessageProcessor(Message msg) {
        switch (msg.what){
            case OnRecvMsg_WindowCreated:
                onWindowCreated();
                break;
            case OnRecvMsg_WindowDestroyed:
                onWindowDestroyed();
                break;
            default:
                break;
        }
    }

}
