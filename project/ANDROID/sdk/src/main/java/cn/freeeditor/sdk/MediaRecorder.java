package cn.freeeditor.sdk;

import android.view.SurfaceHolder;
import android.view.SurfaceView;


public class MediaRecorder extends JNIContext implements SurfaceHolder.Callback {

    private static final int Record_SetConfig = 0;
    private static final int Record_StartCapture = 1;
    private static final int Record_StartRecord = 2;
    private static final int Record_StopCapture = 3;
    private static final int Record_StopRecord = 4;
    private static final int Record_ChangeCameraConfig = 5;
    private static final int Record_ChangeEncodeConfig = 6;

    private long recorderContext;

    private SurfaceView mVideoView;
    private VideoView videoView;

    public MediaRecorder(){
        recorderContext = MediaContext.Instance().createRecorder();
        connectContext(recorderContext);
        String configStr = MediaContext.Instance().getRecorderConfig();
        putJson(Record_SetConfig, configStr);
    }

    public void release(){
        MediaContext.Instance().deleteContext(recorderContext);
        super.release();
        if (videoView != null){
            videoView.release();
        }
    }

    public void startCapture(){
        putMessage(Record_StartCapture);
    }

    @Override
    public Object onGetObject(int key) {
        return null;
    }

    @Override
    public String onGetJson(int key) {
        return null;
    }

    @Override
    public long onGetPointer(int key) {
        return 0;
    }

    @Override
    protected void onPutMessage(JNIMessage msg) {

    }

    @Override
    protected JNIMessage onGetMessage(int key) {
        return null;
    }

    public void startPreview(SurfaceView view){
        videoView = new VideoView(view);
        putPointer(7, videoView.getJniContext());
//        mVideoView = view;
//        if (mVideoView != null) {
//            mVideoView.setVisibility(View.INVISIBLE);
//            mVideoView.getHolder().addCallback(this);
//            mVideoView.setVisibility(View.VISIBLE);
//        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        putObject(7, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
