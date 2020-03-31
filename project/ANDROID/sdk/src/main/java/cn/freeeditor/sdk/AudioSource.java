package cn.freeeditor.sdk;

import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class AudioSource extends JNIContext
        implements IDeviceCallback {

    protected static final String TAG = "AudioSource";

    private int mStatus;
    private JSONObject mConfig;
    private Microphone microphone;

    public AudioSource(){
        mStatus = MediaStatus.Status_Closed;
        microphone = new Microphone();
        microphone.setCallback(this);
        startHandler(getClass().getName());
    }

    public void release(){
        msgHandler.sendEmptyMessage(MsgKey.Media_Close);
        stopHandler();
        mConfig.clear();
        mConfig = null;
    }

    public void open(String cfg){
        if (mStatus != MediaStatus.Status_Closed){
            Log.e(TAG, "[AudioSource open] device is on: " + mStatus);
            return ;
        }
        mConfig = JSON.parseObject(cfg);
        int mode = Microphone.MODE_MICROPHONE;
        String audioDevice = mConfig.getString(MediaConfig.AUDIO_DEVICE);
        int sampleRate = mConfig.getIntValue(MediaConfig.AUDIO_SRC_SAMPLE_RATE);
        int channelCount = mConfig.getIntValue(MediaConfig.AUDIO_SRC_CHANNEL_COUNT);
        int bytesPerSample = mConfig.getIntValue(MediaConfig.AUDIO_BYTES_PER_SAMPLE);
        int samplesPerFrame = mConfig.getIntValue(MediaConfig.AUDIO_SAMPLES_PER_FRAME);
        if (audioDevice.endsWith(MediaConfig.AUDIO_DEVICE_VOICE_CALL)){
            mode = Microphone.MODE_VOICE_CALL;
        }
        microphone.open(sampleRate, channelCount, bytesPerSample, samplesPerFrame, mode);
        if (microphone.isOpened()){
            mStatus = MediaStatus.Status_Opened;
            mConfig.put(MediaConfig.AUDIO_SRC_SAMPLES_PER_FRAME, microphone.getSamplesPerFrame());
            sendMessage(MsgKey.Media_ProcessEvent, mStatus, mConfig.toString());
        }
    }


    public void start(){
        if (mStatus == MediaStatus.Status_Opened || mStatus == MediaStatus.Status_Stopped){
            if (microphone != null){
                microphone.start();
                microphone.setCallback(this);
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
    public void onProcessData(byte[] data, int length) {
        sendMessage(MsgKey.Media_ProcessData, data, length);
    }

    @Override
    public void onError(String error) {
        mStatus = MediaStatus.Status_Error;
        JSONObject js = new JSONObject();
        js.put("error", error);
        sendMessage(MsgKey.Media_ProcessEvent, mStatus, js.toJSONString());
    }
}
