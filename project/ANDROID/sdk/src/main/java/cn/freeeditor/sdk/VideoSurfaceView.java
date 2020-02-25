package cn.freeeditor.sdk;


import android.os.Message;
import android.view.SurfaceHolder;
import android.view.SurfaceView;


public class VideoSurfaceView extends JNIContext implements SurfaceHolder.Callback {

    private SurfaceView surfaceView;

    private static final int PutMsg_SurfaceCreated = 1;
    private static final int PutMsg_SurfaceChanged = 2;
    private static final int PutMsg_SurfaceDestroyed = 3;

    private static final int OnPutMsg_RegisterCallback = 1;


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

    private void addSurfaceHolderCallback(){
        if (surfaceView != null) {
            surfaceView.getHolder().addCallback(this);
        }
    }

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

}
