package cn.freeeditor.sdk;


import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class VideoView extends JNIContext implements SurfaceHolder.Callback {

    private SurfaceView surfaceView;

    private long messageContext;

    public VideoView(SurfaceView view){
        messageContext = MediaContext.Instance().newVideoViewContext();
        setMessageContext(messageContext);
        surfaceView = view;
    }

    @Override
    public void release() {
        MediaContext.Instance().deleteContext(messageContext);
        super.release();
    }

    @Override
    public long getMessageContext() {
        return messageContext;
    }

    @Override
    public void onPutObject(int key, Object obj) {

    }

    @Override
    public void onPutMessage(int key, String msg) {
        if (key == 1){
            if (surfaceView != null) {
//            surfaceView.setVisibility(View.INVISIBLE);
                surfaceView.getHolder().addCallback(this);
//            surfaceView.setVisibility(View.VISIBLE);
            }
        }
    }

    @Override
    public void onPutContext(int key, long ctx) {

    }

    @Override
    public Object onGetObject(int key) {
        return null;
    }

    @Override
    public String onGetMessage(int key) {
        return null;
    }

    @Override
    public long onGetContext(int key) {
        return 0;
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        putObject(1, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        putMessage(2, "{width:" + width + ", height:" + height +"}");
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        putMessage(3, null);
    }
}
