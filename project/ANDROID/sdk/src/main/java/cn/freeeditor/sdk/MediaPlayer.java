package cn.freeeditor.sdk;

import android.view.SurfaceView;

import com.alibaba.fastjson.JSON;

public class MediaPlayer extends JNIContext {
    protected static final String TAG = "MediaPlayer";

    private static final int MsgKey_Exit = 0;
    private static final int MsgKey_Open = 1;
    private static final int MsgKey_Start = 2;
    private static final int MsgKey_Stop = 3;
    private static final int MsgKey_Close = 4;
    private static final int MsgKey_ProcessData = 5;
    private static final int MsgKey_ProcessEvent = 6;
    private static final int MsgKey_ProcessControl = 10;
    private static final int MsgKey_ProcessControlSetWindow = 1001;

    private long playerContext;
    private VideoWindow videoView;

    private String mUrl;

    public MediaPlayer(){
        startHandler(getClass().getName());
        videoView = new VideoWindow();
        playerContext = MediaContext.Instance().connectPlayer();
        connectContext(playerContext);
    }

    public void release(){
        sendMessage(MsgKey_Close);
        disconnectContext();
        super.release();
        MediaContext.Instance().disconnectPlayer(playerContext);
        if (videoView != null){
            videoView.release();
        }
        stopHandler();
    }

    public void setSurfaceView(SurfaceView view){
        videoView.setSurfaceView(view);
    }

    public void open(String url){
        sendMessage(MsgKey_Open, url);
    }

    public void close(){
        sendMessage(MsgKey_Close);
    }

    public void start(){
        sendMessage(MsgKey_ProcessControlSetWindow, videoView.getContextPointer());
        sendMessage(MsgKey_Start);
    }

    public void stop(){
        sendMessage(MsgKey_Stop);
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return new JNIMessage();
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {

    }
}
