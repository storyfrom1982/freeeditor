package cn.freeeditor.sdk;

import android.view.SurfaceView;

public class MediaPlayer extends JNIContext {
    protected static final String TAG = "MediaPlayer";

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
        sendMessage(MsgKey.Media_Close);
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
        sendMessage(MsgKey.Media_Open, url);
    }

    public void close(){
        sendMessage(MsgKey.Media_Close);
    }

    public void start(){
        sendMessage(MsgKey_ProcessControlSetWindow, videoView.getContextPointer());
        sendMessage(MsgKey.Media_Start);
    }

    public void stop(){
        sendMessage(MsgKey.Media_Stop);
    }

    @Override
    protected JNIMessage onRequestMessage(int key) {
        return obtainMessage(0);
    }

//    @Override
//    protected void onRecvMessage(JNIMessage msg) {
//
//    }
}
