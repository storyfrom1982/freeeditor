package cn.freeeditor.sdk;

import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;




public class MediaRecord extends JNIContext implements SurfaceHolder.Callback {

    private static final int Record_SetConfig = 0;
    private static final int Record_StartCapture = 1;
    private static final int Record_StartRecord = 2;
    private static final int Record_StopCapture = 3;
    private static final int Record_StopRecord = 4;
    private static final int Record_ChangeCameraConfig = 5;
    private static final int Record_ChangeEncodeConfig = 6;

    private long messageContext;

    private SurfaceView mVideoView;
    private VideoView videoView;

    public MediaRecord(){
        messageContext = MediaContext.Instance().getContext(MediaContext.Cmd_GetRecord);
        setMessageContext(messageContext);
        String configStr = MediaContext.Instance().getMessage(MediaContext.Cmd_GetRecordConfig);
        putMessage(Record_SetConfig, configStr);
    }

    public void release(){
        MediaContext.Instance().deleteContext(messageContext);
        super.release();
        if (videoView != null){
            videoView.release();
        }
    }

    public void startCapture(){
        putMessage(Record_StartCapture, null);
    }

    @Override
    public void onPutObject(int type, Object obj) {

    }

    @Override
    public void onPutMessage(int key, String msg) {
        Log.e("MediaRecord", msg);
    }

    @Override
    public void onPutContext(int key, long messageContext) {

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

    public void startPreview(SurfaceView view){
        videoView = new VideoView(view);
        putContext(7,videoView.getMessageContext());
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
