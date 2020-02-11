package cn.freeeditor.sdk;


import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class VideoView extends JNIContext implements SurfaceHolder.Callback {

    private SurfaceView surfaceView;

//    private long messageContext;

    public VideoView(SurfaceView view){
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
        if (msg.key == 1){
            if (surfaceView != null) {
//            surfaceView.setVisibility(View.INVISIBLE);
                surfaceView.getHolder().addCallback(this);
//            surfaceView.setVisibility(View.VISIBLE);
            }
        }
    }

    @Override
    protected JNIMessage onGetMessage(int key) {
        return new JNIMessage();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        putObject(1, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        putJson(2, "{width:" + width + ", height:" + height +"}");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        putJson(3, null);
    }
}
