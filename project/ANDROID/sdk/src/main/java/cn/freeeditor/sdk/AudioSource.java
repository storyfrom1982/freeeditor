package cn.freeeditor.sdk;

import android.media.AudioRecord;
import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.util.concurrent.atomic.AtomicBoolean;

public class AudioSource extends JNIContext {


    protected static final String TAG = "AudioSource";

    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;

    private final AtomicBoolean isRunning = new AtomicBoolean(false);
    private final AtomicBoolean isStopped = new AtomicBoolean(true);
    private final AtomicBoolean isStopping = new AtomicBoolean(false);

    private Thread mRecordThread = null;
    private AudioRecord mAudioRecord = null;

    private Microphone microphone;

    public AudioSource(){
        microphone = new Microphone();
        startHandler();
    }

    public void relase(){
        stopHandler();
        super.release();
    }

    public int open(String cfgStr){
        JSONObject cfg = JSON.parseObject(cfgStr);
        mSampleRate = cfg.getIntValue("sampleRate");
        mChannelCount = cfg.getIntValue("channelCount");
        mBytesPerSample = cfg.getIntValue("bitPerSample") / 8;
        mSamplesPerFrame = cfg.getIntValue("samplePerFrame");
        return microphone.open(mSampleRate, mChannelCount, mBytesPerSample, mSamplesPerFrame);
    }


    public void start(){
        microphone.start();
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

}
