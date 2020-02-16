package cn.freeeditor.sdk;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Process;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.util.concurrent.atomic.AtomicBoolean;

public class Microphone extends JNIContext implements Runnable {


    protected static final String TAG = "Microphone";

    private int mMode;
    private int mSampleRate;
    private int mChannelCount;
    private int mBytePerSample;
    private int mSamplePerFrame;
    private int mSampleBufferSize;

    private int framesPerBufferInt;
    private int AudioRecordBufferSize;

    private final AtomicBoolean isRunning = new AtomicBoolean(false);
    private final AtomicBoolean isStopped = new AtomicBoolean(true);
    private final AtomicBoolean isStopping = new AtomicBoolean(false);

    private Thread mRecordThread = null;
    private AudioRecord mAudioRecord = null;


    public int open(String cfgStr){

        android.util.Log.d(TAG,"open ========== enter");

        JSONObject cfg = JSON.parseObject(cfgStr);

        mSampleRate = cfg.getIntValue("sampleRate");
        mChannelCount = cfg.getIntValue("channelCount");
        mBytePerSample = cfg.getIntValue("bitPerSample") / 8;
        mSamplePerFrame = cfg.getIntValue("samplePerFrame");

        try {
            AudioManager am = (AudioManager) MediaContext.Instance().getAppContext().
                    getSystemService(Context.AUDIO_SERVICE);
            String framesPerBuffer = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
            if (framesPerBuffer != null){
                framesPerBufferInt = Integer.parseInt(framesPerBuffer);
            }
        }catch (Exception e){
            framesPerBufferInt = 0;
            e.printStackTrace();
        }

        android.util.Log.d(TAG,"framesPerBuffer size ========== " + framesPerBufferInt);

        if (framesPerBufferInt == 0){
            framesPerBufferInt = 256; // Use default
        }

        int frameSize = mChannelCount * mBytePerSample;
        int channelConfig = mChannelCount == 2 ? AudioFormat.CHANNEL_IN_STEREO : AudioFormat.CHANNEL_IN_MONO;
        int audioFormat = mBytePerSample == 2 ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int minBufferSize = AudioRecord.getMinBufferSize(mSampleRate, channelConfig, audioFormat);

        android.util.Log.v(TAG, "Audio record: wanted " + (mChannelCount == 2 ? "stereo" : "mono")
                + " " + (mBytePerSample == 2 ? "16-bit" : "8-bit") + " " + (mSampleRate / 1000f)
                + "kHz, " + mSamplePerFrame + " frames buffer");

        mSampleBufferSize = mSamplePerFrame * frameSize;
        AudioRecordBufferSize = framesPerBufferInt * frameSize;

        if (minBufferSize < AudioRecordBufferSize << 1){
            minBufferSize = AudioRecordBufferSize << 1;
        }

        if (mMode != android.media.MediaRecorder.AudioSource.VOICE_COMMUNICATION && mMode != android.media.MediaRecorder.AudioSource.DEFAULT && mMode != android.media.MediaRecorder.AudioSource.CAMCORDER){
            mMode = MediaRecorder.AudioSource.MIC;
        }
        //mMode = MediaRecorder.AudioSource.VOICE_COMMUNICATION; //tkp JAVA
        android.util.Log.i(TAG, "[JAVAAudioCapture] DEVSET-AudioSourceType =  " + mMode);
        mAudioRecord = new AudioRecord(mMode, mSampleRate,
                channelConfig, audioFormat, minBufferSize);

        // see notes about AudioTrack state in audioOpen(), above. Probably also applies here.
        if (mAudioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
            android.util.Log.e(TAG, "Failed during initialization of AudioRecord");
            mAudioRecord.release();
            mAudioRecord = null;
            return -1;
        }

        android.util.Log.v(TAG, "Audio record: got " + ((mAudioRecord.getChannelCount() >= 2) ? "stereo" : "mono")
                + " " + ((mAudioRecord.getAudioFormat() == AudioFormat.ENCODING_PCM_16BIT) ? "16-bit" : "8-bit")
                + " " + (mAudioRecord.getSampleRate() / 1000f) + "kHz, " + mSamplePerFrame + " frames buffer");

        android.util.Log.d(TAG,"open ========== exit");

        return 0;
    }


    public void start(){
        android.util.Log.d(TAG,"start ========== enter");
        if (isRunning.compareAndSet(false, true)){
            isStopped.set(false);
            mRecordThread = new Thread(this);
            mRecordThread.start();
        }
        android.util.Log.d(TAG,"start ========== exit");
    }


    public void stop(){
        android.util.Log.d(TAG,"stop ========== enter");
        if (isRunning.compareAndSet(true, false)){
            while (!isStopped.get()){
                synchronized (isRunning){
                    try {
                        isRunning.wait(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }
            try {
                mRecordThread.join();
                mRecordThread = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        android.util.Log.d(TAG,"stop ========== exit");
    }


    /** This method is called by CloudSDK using JNI. */
    public void close() {
        android.util.Log.d(TAG,"close ========== enter");
        if (isStopping.compareAndSet(false, true)){
            stop();
            if (mAudioRecord != null) {
                mAudioRecord.release();
                mAudioRecord = null;
            }
            isStopping.set(false);
        }
        android.util.Log.d(TAG,"close ========== exit");
    }

    @Override
    public void run() {

        if (mAudioRecord == null){
            android.util.Log.e(TAG, "Uninitialized audioRecord!!!!");
            isStopped.set(true);
            return;
        }

        Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);

        int result = 0;
        byte[] buffer = new byte[mSampleBufferSize];

        int framePerBuffer = (int)((float)mSampleBufferSize / AudioRecordBufferSize);
        ++framePerBuffer;
        framePerBuffer <<= 1;
        int cacheBufferSize = AudioRecordBufferSize * framePerBuffer;
        int cacheWritePos = 0;
        int cacheReadPos = 0;
        int cacheDataSize = 0;
        byte[] cacheBuffer = new byte[cacheBufferSize];

        // log Escape.
        int iForLog = mAudioRecord.getAudioSource();


        android.util.Log.d(TAG, "cacheBufferSize: " + cacheBufferSize + "  " + cacheBufferSize / AudioRecordBufferSize);

        mAudioRecord.startRecording();

        if (mAudioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
//            onError(CAPTURE_AUDIO_DEVICE_ERROR.getValue(), mNativeInstance);
            isRunning.set(false);
        }

        while (isRunning.get()){

//            Log.e(TAG, "AudioRecord.read enter");
            result = mAudioRecord.read(cacheBuffer, cacheWritePos, AudioRecordBufferSize);
            if (result == AudioRecord.ERROR_INVALID_OPERATION || result != AudioRecordBufferSize){
                android.util.Log.e(TAG, "AudioRecord.read failed " + result);
//                onError(CAPTURE_AUDIO_DEVICE_ERROR.getValue(), mNativeInstance);
                break;
            }
//            Log.e(TAG, "AudioRecord.read exit");

            cacheDataSize += result;
            if ((cacheWritePos += result) >= cacheBufferSize){
                cacheWritePos -= cacheBufferSize;
            }

            while (cacheDataSize >= mSampleBufferSize){
                int remain = cacheBufferSize - cacheReadPos;
                if (remain >= mSampleBufferSize){
                    System.arraycopy(cacheBuffer, cacheReadPos, buffer, 0, mSampleBufferSize);
                }else{
                    System.arraycopy(cacheBuffer, cacheReadPos, buffer, 0, remain);
                    System.arraycopy(cacheBuffer, 0, buffer, remain, mSampleBufferSize - remain);
                }
                cacheDataSize -= mSampleBufferSize;
                if ((cacheReadPos += mSampleBufferSize) >= cacheBufferSize){
                    cacheReadPos -= cacheBufferSize;
                }
//                Log.d(TAG, "CloudAudioCapture ============>>>>>>>>>>>>>> " + mSampleBufferSize);
//                recordFrame(buffer, mSampleBufferSize, mNativeInstance);
                putBuffer(PutMsg_ProcessSound, buffer, mSampleBufferSize);
            }
        }

        Log.d(TAG, "CloudAudioCapture ============>>>>>>>>>>>>>> exit");

        isStopped.set(true);


        if (mAudioRecord != null) {
            mAudioRecord.stop();
        }

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
    protected void onPutMessage(JNIMessage msg) {
        switch (msg.key){

            case OnPutMsg_OpenRecord:
                open(msg.json);
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
    protected JNIMessage onGetMessage(int key) {
        return null;
    }

}
