package cn.freeeditor.sdk;

import android.content.pm.ActivityInfo;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class MediaRecorder extends JNIContext {

    private static final String TAG = "MediaRecorder";

    private static final int Record_SetConfig = 0;
    private static final int Record_StartCapture = 1;
    private static final int Record_StartRecord = 2;
    private static final int Record_StopCapture = 3;
    private static final int Record_StopRecord = 4;
    private static final int Record_ChangeCameraConfig = 5;
    private static final int Record_ChangeEncodeConfig = 6;

    private long recorderContext;

    private SurfaceView mVideoView;
    private VideoSurfaceView videoView;

    public MediaRecorder(){
        recorderContext = MediaContext.Instance().createRecorder();
        connectContext(recorderContext);
        String mediaCfgStr = MediaContext.Instance().getRecorderConfig();

        JSONObject mediaCfg = JSON.parseObject(mediaCfgStr);
        JSONObject videoCfg = mediaCfg.getJSONObject("video");
        int width = videoCfg.getIntValue("width");
        int height = videoCfg.getIntValue("height");
        int orientation = MediaContext.Instance().getScreenOrientation();
        if ((orientation == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
                || orientation == ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT && width > height)
                || (orientation == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
                || orientation == ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE && height > width)){
            videoCfg.put("width", height);
            videoCfg.put("height", width);
        }
        Log.d(TAG, "encoder config: " + JSON.toJSONString(mediaCfg, true));
        sendMessage(new JNIMessage(Record_SetConfig, mediaCfg.toJSONString()));
    }

    public void release(){
        MediaContext.Instance().deleteContext(recorderContext);
        super.release();
        if (videoView != null){
            videoView.release();
        }
    }

    public void setUrl(String url){
        sendMessage(8);
    }

    public void startCapture(){
        sendMessage(Record_StartCapture);
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return null;
    }

    @Override
    protected void onReceiveMessage(JNIMessage msg) {

    }

    public void startPreview(SurfaceView view){
        videoView = new VideoSurfaceView(view);
        sendMessage(new JNIMessage(7, videoView.getContextPointer()));
//        mVideoView = view;
//        if (mVideoView != null) {
//            mVideoView.setVisibility(View.INVISIBLE);
//            mVideoView.getHolder().addCallback(this);
//            mVideoView.setVisibility(View.VISIBLE);
//        }
    }
}
