package cn.freeeditor.sdk;


import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class VideoSurfaceView extends JNIContext implements SurfaceHolder.Callback {

    private SurfaceView surfaceView;

    private static final int PutMsg_SurfaceError = 0;
    private static final int PutMsg_SurfaceCreated = 1;
    private static final int PutMsg_SurfaceChanged = 2;
    private static final int PutMsg_SurfaceDestroyed = 3;

    private static final int OnPutMsg_RegisterCallback = 1;

    public VideoSurfaceView(){
//        messageContext = MediaContext.Instance().newVideoViewContext();
//        connectContext(messageContext);
//        surfaceView = view;
    }

//    @Override
//    public void release() {
////        MediaContext.Instance().deleteContext(messageContext);
//        super.release();
//    }

//    @Override
//    public long getContextPointer() {
//        return messageContext;
//    }

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
    protected void onReceiveMessage(JNIMessage msg) {
        if (msg.key == OnPutMsg_RegisterCallback){
            if (surfaceView != null) {
                surfaceView.getHolder().addCallback(this);
            }
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        sendMessage(new JNIMessage(PutMsg_SurfaceCreated, holder.getSurface()));
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        sendMessage(PutMsg_SurfaceChanged);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        sendMessage(PutMsg_SurfaceDestroyed);
    }
}
