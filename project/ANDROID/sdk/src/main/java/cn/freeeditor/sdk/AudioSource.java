package cn.freeeditor.sdk;

import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class AudioSource extends JNIContext
        implements Microphone.RecordCallback, Microphone.ErrorCallback {

    protected static final String TAG = "AudioSource";

    protected static final String CFG_SRC_AUDIO_DEVICE           = "srcAudioDevice";
    protected static final String CFG_SRC_SAMPLE_RATE            = "srcSampleRate";
    protected static final String CFG_SRC_CHANNEL_COUNT          = "srcChannelCount";
    protected static final String CFG_CODEC_BYTE_PER_SAMPLE      = "codecBytePerSample";
    protected static final String CFG_CODEC_SAMPLE_PER_FRAME     = "codecSamplePerFrame";

    private static final int Status_Closed = 0;
    private static final int Status_Opened = 1;
    private static final int Status_Started = 2;
    private static final int Status_Stopped = 3;

    private String mAudioDevice;
    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;

    private int mStatus;
    private JSONObject mConfig;
    private Microphone microphone;

    public AudioSource(){
        mStatus = Status_Closed;
        microphone = new Microphone();
        microphone.setErrorCallback(this);
        startHandler(getClass().getName());
    }

    public void release(){
        msgHandler.sendEmptyMessage(OnRecvMsg_CloseRecord);
        stopHandler();
    }

    public void open(String cfgStr){
        if (mStatus != Status_Closed){
            return ;
        }
        mConfig = JSON.parseObject(cfgStr);
        Log.d(TAG, "AudioSource config: " + mConfig.toJSONString());
        mAudioDevice = mConfig.getString(CFG_SRC_AUDIO_DEVICE);
        mSampleRate = mConfig.getIntValue(CFG_SRC_SAMPLE_RATE);
        mChannelCount = mConfig.getIntValue(CFG_SRC_CHANNEL_COUNT);
        mBytesPerSample = mConfig.getIntValue(CFG_CODEC_BYTE_PER_SAMPLE);
        mSamplesPerFrame = mConfig.getIntValue(CFG_CODEC_SAMPLE_PER_FRAME);
        microphone.open(mSampleRate, mChannelCount, mBytesPerSample, mSamplesPerFrame);

        mStatus = Status_Opened;
        sendMessage(SendMsg_Opened, this.mConfig.toString());
    }


    public void start(){
        if (mStatus == Status_Opened || mStatus == Status_Stopped){
            if (microphone != null){
                microphone.start();
                microphone.setRecordCallback(this);
                sendMessage(SendMsg_Started);
                mStatus = Status_Started;
            }
        }
    }


    public void stop(){
        if (mStatus == Status_Started){
            if (microphone != null){
                microphone.stop();
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
            if (microphone != null){
                microphone.close();
                mStatus = Status_Closed;
                sendMessage(SendMsg_Closed);
            }
        }
    }

    @Override
    void onFinalRelease(){
        super.release();
    }

    private static final int OnRecvMsg_OpenRecord = 1;
    private static final int OnRecvMsg_CloseRecord = 2;
    private static final int OnRecvMsg_StartRecord = 3;
    private static final int OnRecvMsg_StopRecord = 4;


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
    public void onRecordFrame(byte[] data, long timestamp) {
        sendMessage(SendMsg_ProcessSound, data, timestamp);
    }

    @Override
    public void onError(int error) {

    }
}
