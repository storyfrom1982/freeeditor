package cn.freeeditor.sdk;

import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

public class AudioPlayer extends JNIContext implements Speaker.SpeakerCallback, Speaker.ErrorCallback {

    protected static final String TAG = "AudioPlayer";

    private static final int Status_Closed = 0;
    private static final int Status_Opened = 1;
    private static final int Status_Started = 2;
    private static final int Status_Stopped = 3;

    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;

    private int mStatus;
    private JSONObject mConfig;
    private Speaker speaker;


    public AudioPlayer(){
        mStatus = Status_Closed;
        speaker = new Speaker();
        speaker.setErrorCallback(this);
        startHandler("AudioPlayer");
    }

    public void release(){
        msgHandler.sendEmptyMessage(OnRecvMsg_CloseRecord);
        stopHandler();
    }

    @Override
    void onFinalRelease(){
        super.release();
    }

    public void open(String cfgStr){
        if (mStatus != Status_Closed){
            return ;
        }
        mConfig = JSON.parseObject(cfgStr);
        Log.d(TAG, "AudioSource config: " + mConfig.toJSONString());
        mSampleRate = mConfig.getIntValue("codecSampleRate");
        mChannelCount = mConfig.getIntValue("codecChannelCount");
        mBytesPerSample = mConfig.getIntValue("codecBytePerSample");
        mSamplesPerFrame = mConfig.getIntValue("codecSamplePerFrame");
        speaker.open(mSampleRate, mChannelCount, mBytesPerSample, mSamplesPerFrame, 0);

        mStatus = Status_Opened;
        sendMessage(SendMsg_Opened, this.mConfig.toString());
    }


    public void start(){
        if (mStatus == Status_Opened || mStatus == Status_Stopped){
            if (speaker != null){
                speaker.start();
                speaker.setSpeakerCallback(this);
                sendMessage(SendMsg_Started);
                mStatus = Status_Started;
            }
        }
    }


    public void stop(){
        if (mStatus == Status_Started){
            if (speaker != null){
                speaker.stop();
                sendMessage(SendMsg_Stopped);
                mStatus = Status_Stopped;
            }
        }
    }


    public void close() {
        if (mStatus == Status_Started){
            stop();
        }
        if (mStatus == Status_Opened || mStatus == Status_Stopped){
            if (speaker != null){
                speaker.close();
                mStatus = Status_Closed;
                sendMessage(SendMsg_Closed);
            }
        }
    }

    private static final int OnRecvMsg_OpenRecord = 1;
    private static final int OnRecvMsg_StartRecord = 2;
    private static final int OnRecvMsg_StopRecord = 3;
    private static final int OnRecvMsg_CloseRecord = 4;


    private static final int SendMsg_Opened = 1;
    private static final int SendMsg_Started = 2;
    private static final int SendMsg_Stopped = 3;
    private static final int SendMsg_Closed = 4;
    private static final int SendMsg_ProcessSound = 5;

    @Override
    void onMessageProcessor(Message msg) {
        JNIMessage jmsg = (JNIMessage) msg.obj;
        switch (msg.what){

            case OnRecvMsg_OpenRecord:
                open(jmsg.string);
                break;

            case OnRecvMsg_StartRecord:
                start();
                break;

            case OnRecvMsg_StopRecord:
                stop();
                break;

            case OnRecvMsg_CloseRecord:
                close();
                break;

            default:
                break;

        }
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return new JNIMessage();
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {
        msgHandler.sendMessage(msgHandler.obtainMessage(msg.key, msg));
    }

    @Override
    public void onPlaySample(byte[] data, long timestamp) {
        sendMessage(SendMsg_ProcessSound, data, timestamp);
    }

    @Override
    public void onError(int error) {

    }
}
