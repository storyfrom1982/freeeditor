package cn.freeeditor.sdk;

import android.os.Message;
import android.view.SurfaceView;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class MediaRecorder extends JNIContext {

    private static final String TAG = "MediaRecorder";

    private static final int MsgKey_UpdateConfig = 11;
    private static final int MsgKey_StartPreview = 12;
    private static final int MsgKey_StopPreview = 13;
    private static final int MsgKey_StartRecord = 14;
    private static final int MsgKey_StopRecord = 15;

    private long recorderContext;
    private VideoWindow videoView;

    private String mUrl;
    private JSONObject mConfig;

    public MediaRecorder(){
        startHandler(getClass().getName());
        videoView = new VideoWindow();
        recorderContext = MediaContext.Instance().connectRecorder();
        connectContext(recorderContext);
        String config = MediaContext.Instance().getRecorderConfig();
        mConfig = JSON.parseObject(config);
//        Log.d(TAG, "encoder config: " + JSON.toJSONString(mConfig));
        sendMessage(MsgKey.Media_Open, mConfig.toJSONString());
    }

    public void release(){
        sendMessage(MsgKey.Media_Close);
        disconnectContext();
        super.release();
        MediaContext.Instance().disconnectRecorder(recorderContext);
        if (videoView != null){
            videoView.release();
        }
        stopHandler();
    }

    public void setVideoSize(int width, int height){
        mConfig.getJSONObject("video").put("width", width);
        mConfig.getJSONObject("video").put("height", height);
    }

    public void updateConfig(){
        sendMessage(MsgKey_UpdateConfig, mConfig.toJSONString());
    }

    public void startCapture(){
        sendMessage(MsgKey.Media_Start);
    }

    public void stopCapture(){
        sendMessage(MsgKey.Media_Stop);
    }

    public void startRecord(String url){
        mUrl = url;
        sendMessage(MsgKey_StartRecord, mUrl);
    }

    public void stopRecord(){
        sendMessage(MsgKey_StopRecord);
    }

    public void startPreview(SurfaceView view){
        videoView.setSurfaceView(view);
        sendMessage(MsgKey_StartPreview, videoView.getContextPointer());
    }

    public void stopPreview(){
        sendMessage(MsgKey_StopPreview);
    }

    @Override
    void onFinalRelease() {
    }

    @Override
    protected JNIMessage onRequestMessage(int key) {
        return new JNIMessage();
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {
        msgHandler.sendMessage(msgHandler.obtainMessage(msg.key, msg));
    }

    private void processEvent(JNIMessage msg){
        Log.d(TAG, "processEvent >>>>: " + msg.key);
    }

    @Override
    void onMessageProcessor(Message msg) {
        JNIMessage jmsg = (JNIMessage) msg.obj;
        switch (msg.what){
            case MsgKey.Media_ProcessEvent:
                processEvent(jmsg);
                break;
            default:
                break;

        }
    }
}
