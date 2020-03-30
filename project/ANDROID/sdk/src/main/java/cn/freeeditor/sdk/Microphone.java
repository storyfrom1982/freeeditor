package cn.freeeditor.sdk;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Process;

import java.util.concurrent.atomic.AtomicBoolean;

public class Microphone implements Runnable {

    private static final String TAG = "Microphone";

    public static int MODE_MICROPHONE = 0;
    public static int MODE_VOICE_CALL = 1;

    private int mMode;
    private int mSampleRate;
    private int mChannelCount;
    private int mBytesPerSample;
    private int mSamplesPerFrame;
    private int mRecordBufferSize;

    private final AtomicBoolean isRunning = new AtomicBoolean(false);
    private final AtomicBoolean isStopped = new AtomicBoolean(true);
    private final AtomicBoolean isClosing = new AtomicBoolean(false);

    private Thread mRecordThread = null;
    private AudioRecord mAudioRecord = null;

    private final Object mLock = new Object();
    private IDeviceCallback mCallback;

    public Microphone(){}

    public void setCallback(IDeviceCallback callback){
        synchronized (mLock){
            mCallback = callback;
        }
    }

    public boolean isOpened(){
        return mAudioRecord != null;
    }

    public int getSamplesPerFrame(){
        return mSamplesPerFrame;
    }

    public void open(int sampleRate, int channelCount, int bytesPerSample, int samplesPerFrame, int mode){
        mSampleRate = sampleRate;
        mChannelCount = channelCount;
        mBytesPerSample = bytesPerSample;
        mSamplesPerFrame = samplesPerFrame;
        mSamplesPerFrame = getPropertyOutputFramesPerBuffer();
        mRecordBufferSize = mSamplesPerFrame * mChannelCount * mBytesPerSample;

        int channelConfig = mChannelCount == 2 ? AudioFormat.CHANNEL_IN_STEREO : AudioFormat.CHANNEL_IN_MONO;
        int audioFormat = mBytesPerSample == 2 ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
        int minBufferSize = AudioRecord.getMinBufferSize(mSampleRate, channelConfig, audioFormat);

        if (minBufferSize < mRecordBufferSize << 1){
            minBufferSize = mRecordBufferSize << 1;
        }

        if (mode == MODE_VOICE_CALL){
            mMode = MediaRecorder.AudioSource.VOICE_COMMUNICATION;
        }else {
            mMode = MediaRecorder.AudioSource.MIC;
        }

        mAudioRecord = new AudioRecord(mMode, mSampleRate,
                channelConfig, audioFormat, minBufferSize);

        if (mAudioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
            mAudioRecord.release();
            mAudioRecord = null;
            if (mCallback != null){
                mCallback.onError("microphone open failed");
            }
        }
    }

    public void close(){
        if (isClosing.compareAndSet(false, true)){
            stop();
            if (mAudioRecord != null) {
                mAudioRecord.release();
                mAudioRecord = null;
            }
            isClosing.set(false);
        }
    }

    public void start(){
        if (isRunning.compareAndSet(false, true)){
            isStopped.set(false);
            mRecordThread = new Thread(this);
            mRecordThread.start();
        }
    }

    public void stop(){
        if (isRunning.compareAndSet(true, false)){
            synchronized (isStopped){
                if (!isStopped.get()){
                    try {
                        isStopped.wait();
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
    }

    @Override
    public void run() {

        Log.dumpThread("MessageProcessor", getClass().getName(), "thread start");

        Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);

        byte[] recordBuffer = new byte[mRecordBufferSize];

        mAudioRecord.startRecording();
        int result = mAudioRecord.getRecordingState();
        if (result != AudioRecord.RECORDSTATE_RECORDING) {
            synchronized (mLock){
                if (mCallback != null){
                    mCallback.onError("microphone start failed");
                }
            }
            isRunning.set(false);
        }

        while (isRunning.get()){

            result = mAudioRecord.read(recordBuffer, 0, mRecordBufferSize);
            if (result == AudioRecord.ERROR_INVALID_OPERATION || result != mRecordBufferSize){
                synchronized (mLock){
                    if (mCallback != null){
                        mCallback.onError("microphone read failed");
                    }
                }
                break;
            }
            synchronized (mLock){
                if (mCallback != null){
                    mCallback.onProcessData(recordBuffer, recordBuffer.length);
                }
            }
        }

        Log.dumpThread("MessageProcessor", getClass().getName(), "thread stop");

        synchronized (isStopped){
            isStopped.set(true);
            isStopped.notifyAll();
        }
    }

    private int getPropertyOutputFramesPerBuffer(){
        int framesPerBuffer = 0;
        try {
            AudioManager am = (AudioManager) MediaContext.Instance().getAppContext().
                    getSystemService(Context.AUDIO_SERVICE);
            String str = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
            if (str != null){
                framesPerBuffer = Integer.parseInt(str);
                if (framesPerBuffer < 64){
                    framesPerBuffer = mSamplesPerFrame;
                }
                Log.d(TAG,"PROPERTY_OUTPUT_FRAMES_PER_BUFFER[" + framesPerBuffer + "]");
            }
        }catch (Exception e){
            framesPerBuffer = mSamplesPerFrame;
            e.printStackTrace();
        }
        return framesPerBuffer;
    }

}
