package cn.freeeditor.sdk;

import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;


public class AudioSource extends JNIContext
        implements Microphone.RecordCallback, Microphone.ErrorCallback {

    protected static final String TAG = "AudioSource";

    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;

    private Microphone microphone;

    public AudioSource(){
        microphone = new Microphone();
        microphone.setErrorCallback(this);
        startHandler();
    }

    public void release(){
        Log.d(TAG, "AudioSource ########################### release()");
        msgHandler.sendEmptyMessage(OnPutMsg_CloseRecord);
        stopHandler();
    }

    public int open(String cfgStr){
        JSONObject cfg = JSON.parseObject(cfgStr);
        Log.d(TAG, "AudioSource config: " + cfg.toJSONString());
        mSampleRate = cfg.getIntValue("srcSampleRate");
        mChannelCount = cfg.getIntValue("srcChannelCount");
        mBytesPerSample = cfg.getIntValue("srcBytesPerSample");
        mSamplesPerFrame = cfg.getIntValue("codecSamplesPerFrame");
        return microphone.open(mSampleRate, mChannelCount, mBytesPerSample, mSamplesPerFrame);
    }


    public void start(){
        microphone.start();
        microphone.setRecordCallback(this);
    }


    public void stop(){
        microphone.stop();
    }


    public void close() {
        microphone.close();
    }

    @Override
    void onFinalRelease(){
        super.release();
    }

    private static final int OnPutMsg_OpenRecord = 1;
    private static final int OnPutMsg_CloseRecord = 2;
    private static final int OnPutMsg_StartRecord = 3;
    private static final int OnPutMsg_StopRecord = 4;


    private static final int PutMsg_Opened = 1;
    private static final int PutMsg_Closed = 2;
    private static final int PutMsg_Started = 3;
    private static final int PutMsg_Stopped = 4;
    private static final int PutMsg_ProcessSound = 5;

    @Override
    void onMessageProcessor(Message msg) {
        JNIMessage jmsg = (JNIMessage) msg.obj;
        switch (msg.what){

            case OnPutMsg_OpenRecord:
                open(jmsg.string);
                break;

            case OnPutMsg_StartRecord:
                start();
                break;

            case OnPutMsg_StopRecord:
                stop();
                break;

            case OnPutMsg_CloseRecord:
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
    public void onRecordFrame(byte[] data, int size) {
        sendMessage(PutMsg_ProcessSound, size);
    }

    @Override
    public void onError(int error) {

    }
}
