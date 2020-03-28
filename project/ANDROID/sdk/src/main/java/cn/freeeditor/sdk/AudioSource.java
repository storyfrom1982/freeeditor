package cn.freeeditor.sdk;

import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class AudioSource extends JNIContext
        implements Microphone.RecordCallback, Microphone.ErrorCallback {

    protected static final String TAG = "AudioSource";

    private String mAudioDevice;
    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;

    private int mStatus;
    private JSONObject mConfig;
    private Microphone microphone;

    public AudioSource(){
        mStatus = MediaStatus.Status_Closed;
        microphone = new Microphone();
        microphone.setErrorCallback(this);
        startHandler(getClass().getName());
    }

    public void release(){
        msgHandler.sendEmptyMessage(MsgKey.Media_Close);
        stopHandler();
    }

    public void open(String cfgStr){
        if (mStatus != MediaStatus.Status_Closed){
            return ;
        }
        mConfig = JSON.parseObject(cfgStr);
        Log.d(TAG, "AudioSource config: " + mConfig.toJSONString());
        mAudioDevice = mConfig.getString(MediaConfig.AUDIO_DEVICE);
        mSampleRate = mConfig.getIntValue(MediaConfig.AUDIO_SRC_SAMPLE_RATE);
        mChannelCount = mConfig.getIntValue(MediaConfig.AUDIO_SRC_CHANNEL_COUNT);
        mBytesPerSample = mConfig.getIntValue(MediaConfig.AUDIO_BYTE_PER_SAMPLE);
        mSamplesPerFrame = mConfig.getIntValue(MediaConfig.AUDIO_SAMPLE_PER_FRAME);
        microphone.open(mSampleRate, mChannelCount, mBytesPerSample, mSamplesPerFrame);

        mStatus = MediaStatus.Status_Opened;
        sendMessage(MsgKey.Media_ProcessEvent, mStatus, mConfig.toString());
    }


    public void start(){
        if (mStatus == MediaStatus.Status_Opened || mStatus == MediaStatus.Status_Stopped){
            if (microphone != null){
                microphone.start();
                microphone.setRecordCallback(this);
                mStatus = MediaStatus.Status_Started;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }


    public void stop(){
        if (mStatus == MediaStatus.Status_Started){
            if (microphone != null){
                microphone.stop();
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
            if (microphone != null){
                microphone.close();
                mStatus = MediaStatus.Status_Closed;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }

    @Override
    void onFinalRelease(){
        super.release();
    }

    @Override
    void onMessageProcessor(Message msg) {
        JNIMessage jmsg = (JNIMessage) msg.obj;
        switch (msg.what){

            case MsgKey.Media_Open:
                open(jmsg.string);
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
        return new JNIMessage();
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {
        msgHandler.sendMessage(msgHandler.obtainMessage(msg.key, msg));
    }

    @Override
    public void onRecordFrame(byte[] data, int length) {
        sendMessage(MsgKey.Media_ProcessData, data, length);
    }

    @Override
    public void onError(int error) {

    }
}
