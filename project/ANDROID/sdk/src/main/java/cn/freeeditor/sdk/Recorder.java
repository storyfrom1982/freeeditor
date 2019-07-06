package cn.freeeditor.sdk;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public class Recorder implements IRecorder, Runnable, SurfaceHolder.Callback {

    private static final String TAG = "Recorder";

    //send msg
    private static final int MSG_SendReq_LoadConfig = 0;
    private static final int MSG_SendReq_SaveConfig = 1;
    private static final int MSG_SendCmd_SetVideoSource = 2;
    private static final int MSG_SendCmd_RemoveVideoSource = 3;
    private static final int MSG_SendCmd_SetAudioSource = 4;
    private static final int MSG_SendCmd_RemoveAudioSource = 5;
    private static final int MSG_SendCmd_StartPreview = 6;
    private static final int MSG_SendCmd_StopPreview = 7;
    private static final int MSG_SendCmd_StartPushStream = 8;
    private static final int MSG_SendCmd_StopPushStream = 9;
    private static final int MSG_RecvCmd_SetVideoView = 10;

    //receive msg
    private static final int MSG_RecvReq_GetCamera = 0;
    private static final int MSG_RecvReq_GetMicrophone = 1;


    private SurfaceView mVideoView;
    private IVideoSource mVideoSource;

    private final Thread mThread;
    private final long mNativeInstance;
    private final MsgHandler mMsgHandler;
    private MsgHandler.IMsgListener mMsgListener;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    private JSONObject mConfig;

    Recorder(long nativeListener) {
        mNativeInstance = nativeListener;
        mMsgHandler = new MsgHandler(new MsgHandler.IMsgListener() {
            @Override
            public void onMessage(Msg msg) {
                mThreadHandler.sendMessage(mThreadHandler.obtainMessage(MSG_HandleMessage, msg));
            }

            @Override
            public Msg onRequest(Msg msg) {
                return Recorder.this.onRequest(msg);
            }
        });

        mThread = new Thread(this);
        mThread.start();

        if (!isRunning.get()){
            synchronized (isRunning){
                try {
                    isRunning.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        mMsgHandler.setListener(mNativeInstance);

        Msg msg = mMsgHandler.requestMessage(new Msg(MSG_SendReq_LoadConfig));
        try {
            mConfig = new JSONObject(msg.msgToString());
            Log.d(TAG, mConfig.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }

//        mMsgHandler.requestMessage(new Msg(MSG_SendReq_SaveConfig, mConfig.toString()));

        mVideoSource = new VideoCamera();
//        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_SetVideoSource, mVideoSource.getInstance()));
//        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_StartPreview));
    }

    public void remove(){
        mThreadHandler.sendEmptyMessage(MSG_HandleRemove);
        try {
            mThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    public long getNativeInstance() {
        return mNativeInstance;
    }

    @Override
    public void setListener(MsgHandler.IMsgListener listener) {
        mThreadHandler.sendMessage(mThreadHandler.obtainMessage(MSG_HandleSetListener, listener));
    }

    @Override
    public void setResolution(int width, int height) {
        try {
            String cfg = mConfig.getJSONObject("videoSource").put("width", width).put("height", height).toString();
            Log.d(TAG, cfg);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void startPreview() {
        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_SetVideoSource, mVideoSource.getInstance()));
        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_StartPreview));
    }

    @Override
    public void stopPreview() {

    }

    @Override
    public void startPushStream() {
        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_StartPushStream));
    }

    @Override
    public void stopPushStream() {
        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_StopPushStream));
    }

    @Override
    public void setPreviewSurface(SurfaceView view) {
        mVideoView = view;
        if (mVideoSource != null){
            mVideoView.getHolder().addCallback(this);
        }
    }


    private Msg onRequest(Msg msg){
        switch (msg.key){
            case MSG_RecvReq_GetCamera:
                break;
            default:
                break;
        }
        return new Msg(-1);
    }

    private void internalSetListener(MsgHandler.IMsgListener listener){
        mMsgListener = listener;
    }

    private void internalRemove(){
//        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_RemoveVideoSource));
//        mMsgHandler.sendMessage(new Msg(MSG_SendCmd_RemoveAudioSource));
        mMsgHandler.remove();
        if (mVideoSource != null){
            mVideoSource.remove();
            mVideoSource = null;
        }
        Looper.myLooper().quit();
    }

    private void handleMessage(Msg msg){
        if (mMsgListener != null){
            mMsgListener.onMessage(msg);
        }
    }

    private static final int MSG_HandleSetListener = 0;
    private static final int MSG_HandleRemove = 1;
    private static final int MSG_HandleMessage = 2;

    private ThreadHandler mThreadHandler;

    @Override
    public void run() {
        Looper.prepare();
        mThreadHandler = new ThreadHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mMsgHandler.sendMessage(new Msg(MSG_RecvCmd_SetVideoView, holder.getSurface(), 0));
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    private static final class ThreadHandler extends Handler {

        final WeakReference<Recorder> weakReference;

        ThreadHandler(Recorder recorder){
            weakReference = new WeakReference<>(recorder);
        }

        @Override
        public void handleMessage(Message msg) {
            Recorder recorder = weakReference.get();
            if (recorder != null){
                switch (msg.what){
                    case MSG_HandleSetListener:
                        recorder.internalSetListener((MsgHandler.IMsgListener) msg.obj);
                        break;
                    case MSG_HandleRemove:
                        recorder.internalRemove();
                        break;
                    default:
                        recorder.handleMessage((Msg) msg.obj);
                }
            }
        }
    }

}
