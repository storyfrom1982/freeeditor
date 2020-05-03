package cn.freeeditor.sdk;

import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

public class AudioPlayer extends JNIContext implements Speaker.SpeakerCallback, Speaker.ErrorCallback {

    protected static final String TAG = "AudioPlayer";

    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;

    private int mStatus;
    private JSONObject mConfig;
    private Speaker speaker;


    public AudioPlayer(){
        mStatus = MediaStatus.Status_Closed;
        speaker = new Speaker();
        speaker.setErrorCallback(this);
        startHandler(getClass().getName());
    }

    public void release(){
//        msgHandler.sendEmptyMessage(MsgKey.Media_Close);
        stopHandler();
    }

    @Override
    void onFinalRelease(){
        super.release();
    }

    public void open(String cfgStr){
        if (mStatus != MediaStatus.Status_Closed){
            return ;
        }
        mConfig = JSON.parseObject(cfgStr);
        Log.d(TAG, "AudioPlayer config: " + mConfig.toJSONString());
        mSampleRate = mConfig.getIntValue(MediaConfig.AUDIO_SAMPLE_RATE);
        mChannelCount = mConfig.getIntValue(MediaConfig.AUDIO_CHANNEL_COUNT);
        mBytesPerSample = mConfig.getIntValue(MediaConfig.AUDIO_BYTES_PER_SAMPLE);
        mSamplesPerFrame = mConfig.getIntValue(MediaConfig.AUDIO_SAMPLES_PER_FRAME);
        speaker.open(mSampleRate, mChannelCount, mBytesPerSample, mSamplesPerFrame, 0);

        mStatus = MediaStatus.Status_Opened;
        sendMessage(MsgKey.Media_ProcessEvent, mStatus, mConfig.toString());
    }


    public void start(){
        if (mStatus == MediaStatus.Status_Opened || mStatus == MediaStatus.Status_Stopped){
            if (speaker != null){
                speaker.start();
                speaker.setSpeakerCallback(this);
                mStatus = MediaStatus.Status_Started;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }


    public void stop(){
        if (mStatus == MediaStatus.Status_Started){
            if (speaker != null){
                speaker.stop();
                mStatus = MediaStatus.Status_Stopped;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }


    public void close() {
        if (mStatus == MediaStatus.Status_Started){
            stop();
        }
        if (mStatus == MediaStatus.Status_Opened || mStatus == MediaStatus.Status_Stopped){
            if (speaker != null){
                speaker.close();
                mStatus = MediaStatus.Status_Closed;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }

    @Override
    void onMessageProcessor(JNIMessage msg) {
        switch (msg.getKey()){

            case MsgKey.Media_Open:
                open(msg.getString());
                break;

            case MsgKey.Media_Start:
                start();
                break;

            case MsgKey.Media_Stop:
                stop();
                break;

            case MsgKey.Media_Close:
                close();
                break;

            default:
                break;

        }
    }

    @Override
    protected JNIMessage onRequestMessage(int key) {
        return obtainMessage(0);
    }

//    @Override
//    protected void onRecvMessage(JNIMessage msg) {
//        msgHandler.sendMessage(msgHandler.obtainMessage(msg.key, msg));
//    }

    @Override
    public void onPlaySample(byte[] data, int length) {
        sendMessage(MsgKey.Media_ProcessData, data, length);
    }

    @Override
    public void onError(int error) {

    }
}
