package cn.freeeditor.sdk;

import android.view.SurfaceView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class MediaRecorder extends JNIContext {

    private static final String TAG = "MediaRecorder";

    private static final int SendMsg_None = 0;
    private static final int SendMsg_Open = 1;
    private static final int SendMsg_Start = 2;
    private static final int SendMsg_Stop = 3;
    private static final int SendMsg_Close = 4;
    private static final int SendMsg_StartRecord = 5;
    private static final int SendMsg_StopRecord = 6;
    private static final int SendMsg_StartPreview = 7;
    private static final int SendMsg_StopPreview = 8;
    private static final int SendMsg_UpdateConfig = 9;

    private long recorderContext;
    private VideoSurfaceView videoView;

    private String mUrl;
    private JSONObject mConfig;

    public MediaRecorder(){
        videoView = new VideoSurfaceView();
        recorderContext = MediaContext.Instance().createRecorder();
        connectContext(recorderContext);
        String config = MediaContext.Instance().getRecorderConfig();
        mConfig = JSON.parseObject(config);
        Log.d(TAG, "encoder config: " + JSON.toJSONString(mConfig, true));
        sendMessage(SendMsg_Open, mConfig.toJSONString());
    }

    public void setVideoSize(int width, int height){
        mConfig.getJSONObject("video").put("width", width);
        mConfig.getJSONObject("video").put("height", height);
    }

    public void updateConfig(){
        sendMessage(SendMsg_UpdateConfig, mConfig.toJSONString());
    }

    public void startCapture(){
        sendMessage(SendMsg_Start);
    }

    public void stopCapture(){
        sendMessage(SendMsg_Stop);
    }

    public void startRecord(String url){
        mUrl = url;
        sendMessage(SendMsg_StartRecord, mUrl);
    }

    public void stopRecord(){
        sendMessage(SendMsg_StopRecord);
    }

    public void startPreview(SurfaceView view){
        videoView.setSurfaceView(view);
        sendMessage(SendMsg_StartPreview, videoView.getContextPointer());
    }

    public void stopPreview(){
        sendMessage(SendMsg_StopPreview);
    }

    public void release(){
        sendMessage(SendMsg_Close);
        MediaContext.Instance().deleteContext(recorderContext);
        super.release();
        if (videoView != null){
            videoView.release();
        }
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return new JNIMessage();
    }

    @Override
    protected void onReceiveMessage(JNIMessage msg) {

    }

}
