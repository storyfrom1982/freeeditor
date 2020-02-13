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

    public VideoSurfaceView(SurfaceView view){
//        messageContext = MediaContext.Instance().newVideoViewContext();
//        connectContext(messageContext);
        surfaceView = view;
    }

//    @Override
//    public void release() {
////        MediaContext.Instance().deleteContext(messageContext);
//        super.release();
//    }

//    @Override
//    public long getJniContext() {
//        return messageContext;
//    }

    @Override
    protected void onPutMessage(JNIMessage msg) {
        if (msg.key == OnPutMsg_RegisterCallback){
            if (surfaceView != null) {
                surfaceView.getHolder().addCallback(this);
            }
        }
    }

    @Override
    protected JNIMessage onGetMessage(int key) {
        return new JNIMessage();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        putObject(PutMsg_SurfaceCreated, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        putMessage(PutMsg_SurfaceChanged);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        putMessage(PutMsg_SurfaceDestroyed);
    }
}
