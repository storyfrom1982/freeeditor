package cn.freeeditor.sdk;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import org.json.JSONException;
import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.concurrent.atomic.AtomicBoolean;

public class Editor implements IEditor, Runnable, SurfaceHolder.Callback {

    private static final String TAG = "Editor";


    private SurfaceView mVideoView;
    private IVideoSource mVideoSource;

    private final Thread mThread;
    private final long mNativeInstance;
    private final MsgHandler mMsgHandler;
    private MsgHandler.IMsgListener mMsgListener;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    private JSONObject mConfig;

    Editor(long nativeListener) {
        mNativeInstance = nativeListener;
        mMsgHandler = new MsgHandler(new MsgHandler.IMsgListener() {
            @Override
            public void onReceiveMessage(Msg msg) {
                mThreadHandler.sendMessage(mThreadHandler.obtainMessage(MSG_HandleMessage, msg));
            }

            @Override
            public Msg onReceiveRequest(Msg msg) {
                return Editor.this.onRequest(msg);
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

        Msg msg = mMsgHandler.sendRequest(new Msg(MsgKey.Editor_LoadConfig));
        try {
            mConfig = new JSONObject(msg.msgToString());
            Log.d(TAG, mConfig.toString());
        } catch (JSONException e) {
            e.printStackTrace();
        }

//        mMsgHandler.sendRequest(new Msg(MSG_SendReq_SaveConfig, m_config.toString()));

//        mVideoSource = new VideoSource();
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
        mMsgHandler.sendMessage(new Msg(MsgKey.Editor_OpenVideoSource, mVideoSource.getInstance()));
        mMsgHandler.sendMessage(new Msg(MsgKey.Editor_StartPreview));
    }

    @Override
    public void stopPreview() {

    }

    @Override
    public void startPushStream() {
        mMsgHandler.sendMessage(new Msg(MsgKey.Editor_StartPublish));
    }

    @Override
    public void stopPushStream() {
        mMsgHandler.sendMessage(new Msg(MsgKey.Editor_StopPublish));
    }

    @Override
    public void setPreviewSurface(SurfaceView view) {
        mVideoView = view;
        if (mVideoView != null) {
            mVideoView.setVisibility(View.INVISIBLE);
            mVideoView.getHolder().addCallback(this);
            mVideoView.setVisibility(View.VISIBLE);
        }
    }


    private Msg onRequest(Msg msg){
        switch (msg.key){
            case MsgKey.Editor_OpenVideoSource:
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
            mMsgListener.onReceiveMessage(msg);
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
        mMsgHandler.sendMessage(new Msg(MsgKey.Editor_SetVideoView, holder.getSurface()));
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    private static final class ThreadHandler extends Handler {

        final WeakReference<Editor> weakReference;

        ThreadHandler(Editor recorder){
            weakReference = new WeakReference<>(recorder);
        }

        @Override
        public void handleMessage(Message msg) {
            Editor recorder = weakReference.get();
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
