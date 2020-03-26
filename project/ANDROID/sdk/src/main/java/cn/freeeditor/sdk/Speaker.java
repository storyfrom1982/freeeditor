package cn.freeeditor.sdk;

import android.content.Context;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.os.Process;

import java.util.concurrent.atomic.AtomicBoolean;

public class Speaker implements Runnable {

    private static final String TAG = "Speaker";

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

    private Thread mRecordThread = null;
    private AudioTrack mAudioTrack = null;

    public interface SpeakerCallback {
        void onPlaySample(byte[] data, long timestamp);
    }

    public interface ErrorCallback {
        void onError(int error);
    }

    private ErrorCallback errorCallback;
    private SpeakerCallback speakerCallback;

    void setSpeakerCallback(SpeakerCallback callback){
        speakerCallback = callback;
    }

    void setErrorCallback(ErrorCallback callback){
        errorCallback = callback;
    }

    public int open(int sampleRate, int channelCount, int bytePerSample, int desiredFrames, int mode){

        android.util.Log.d(TAG,"open ========== enter");

        android.util.Log.d(TAG,"open ========== stream type " + mode);

        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.JELLY_BEAN_MR1){
            try {
                AudioManager am = (AudioManager) MediaContext.Instance().getAppContext().getSystemService(Context.AUDIO_SERVICE);
                String framesPerBuffer = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
                if (framesPerBuffer != null){
                    framesPerBufferInt = Integer.parseInt(framesPerBuffer);
                }
            }catch (Exception e){
                framesPerBufferInt = 0;
                e.printStackTrace();
            }
            android.util.Log.d(TAG,"framesPerBuffer size ========== " + framesPerBufferInt);
        }

        if (framesPerBufferInt == 0){
            framesPerBufferInt = 256; // Use default
        }

        mMode = mode;
        mSampleRate = sampleRate;
        mChannelCount = channelCount;
//        mBytePerSample = bytePerSample;
        mBytePerSample = 2;
        mSamplePerFrame = desiredFrames;

        int frameSize = mChannelCount * mBytePerSample;
        int channelConfig = mChannelCount == 2 ? AudioFormat.CHANNEL_OUT_STEREO : AudioFormat.CHANNEL_OUT_MONO;
        int audioFormat = mBytePerSample == 2 ? AudioFormat.ENCODING_PCM_16BIT : AudioFormat.ENCODING_PCM_8BIT;
//        audioFormat = AudioFormat.ENCODING_PCM_FLOAT;

        int minBufferSize = AudioRecord.getMinBufferSize(sampleRate, channelConfig, audioFormat);

        Log.d(TAG, "Audio record: wanted " + (mChannelCount == 2 ? "stereo" : "mono")
                + " " + (mBytePerSample == 2 ? "16-bit" : "8-bit") + " " + (mSampleRate / 1000f)
                + "kHz, " + mSamplePerFrame + " frames buffer");

        mSampleBufferSize = mSamplePerFrame * frameSize;
        AudioRecordBufferSize = framesPerBufferInt * frameSize;

        if (minBufferSize < AudioRecordBufferSize << 1){
            minBufferSize = AudioRecordBufferSize << 1;
        }

        if (mMode != AudioManager.STREAM_MUSIC && mMode != AudioManager.STREAM_VOICE_CALL){
            mMode = AudioManager.STREAM_MUSIC;
        }
        mMode = AudioManager.STREAM_MUSIC;

        mAudioTrack = new AudioTrack(mMode, sampleRate,
                channelConfig, audioFormat, minBufferSize, AudioTrack.MODE_STREAM);

        // see notes about AudioTrack state in audioOpen(), above. Probably also applies here.
        if (mAudioTrack.getState() != AudioRecord.STATE_INITIALIZED) {
            android.util.Log.e(TAG, "Failed during initialization of AudioRecord");
            mAudioTrack.release();
            mAudioTrack = null;
            return -1;
        }

        Log.d(TAG, "Audio record: got " + ((mAudioTrack.getChannelCount() >= 2) ? "stereo" : "mono")
                + " " + ((mAudioTrack.getAudioFormat() == AudioFormat.ENCODING_PCM_FLOAT) ? "32-bit" : "8-bit")
                + " " + (mAudioTrack.getSampleRate() / 1000f) + "kHz, " + mSamplePerFrame + " frames buffer");

        Log.d(TAG,"open ========== exit");

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
                        isRunning.wait(1);
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
        stop();
        if (mAudioTrack != null) {
            mAudioTrack.release();
            mAudioTrack = null;
        }
        Log.d(TAG,"close ========== exit");
    }

    @Override
    public void run() {
        Log.dumpThread("MessageProcessor", getClass().getName(), "thread start");

        Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);

        AudioManager am = (AudioManager) MediaContext.Instance().getAppContext().getSystemService(Context.AUDIO_SERVICE);
        int result = am.requestAudioFocus(audioFocusChangeListener, mMode, AudioManager.AUDIOFOCUS_GAIN);
        if (result == AudioManager.AUDIOFOCUS_REQUEST_GRANTED){
            Log.e(TAG, "AudioManager.AUDIOFOCUS_REQUEST_GRANTED");
        }else{
            Log.e(TAG, "AudioManager.AUDIOFOCUS_REQUEST_FAILED");
        }

        byte[] buffer = new byte[mSampleBufferSize];
        Log.d(TAG, "sampleBufferSize: " + mSampleBufferSize);

        int framePerBuffer = (int)((float)mSampleBufferSize / AudioRecordBufferSize);
        ++framePerBuffer;
        framePerBuffer <<= 1;
        int cacheBufferSize = AudioRecordBufferSize * framePerBuffer;
        int cacheWritePos = 0;
        int cacheReadPos = 0;
        int cacheDataSize = 0;
        byte[] cacheBuffer = new byte[cacheBufferSize];

        Log.d(TAG, "cacheBufferSize: " + cacheBufferSize + "  " + cacheBufferSize / AudioRecordBufferSize);

        mAudioTrack.play();

        while (isRunning.get()) {

            if (speakerCallback != null){
                speakerCallback.onPlaySample(buffer, buffer.length);
            }

            int remain = cacheBufferSize - cacheWritePos;
            if (remain >= buffer.length){
                System.arraycopy(buffer, 0, cacheBuffer, cacheWritePos, buffer.length);
            }else{
                System.arraycopy(buffer, 0, cacheBuffer, cacheWritePos, remain);
                System.arraycopy(buffer, remain, cacheBuffer, 0, buffer.length - remain);
            }
            cacheDataSize += buffer.length;
            if ((cacheWritePos += buffer.length) >= cacheBufferSize){
                cacheWritePos -= cacheBufferSize;
            }

            while (cacheDataSize >= AudioRecordBufferSize){

                for (int i = 0; i < AudioRecordBufferSize; ) {
                    result = mAudioTrack.write(cacheBuffer, cacheReadPos + i, AudioRecordBufferSize - i);
                    if (result > 0) {
                        i += result;
                    } else if (result == 0) {
                        try {
                            Log.e(TAG, "AudioTrack.write sleep");
                            Thread.sleep(1);
                        } catch (InterruptedException e) {
                            // Nom nom
                        }
                    } else {
                        Log.e(TAG, "AudioTrack.write failed " + result);
                        break;
                    }
                }

                cacheDataSize -= AudioRecordBufferSize;
                if ((cacheReadPos += AudioRecordBufferSize) >= cacheBufferSize){
                    cacheReadPos -= cacheBufferSize;
                }
            }
        }


        isStopped.set(true);

        am.abandonAudioFocus(audioFocusChangeListener);


        Log.dumpThread("MessageProcessor", getClass().getName(), "thread stop");

        if (mAudioTrack != null) {
            mAudioTrack.stop();
        }
    }

    private AudioManager.OnAudioFocusChangeListener audioFocusChangeListener = new AudioManager.OnAudioFocusChangeListener() {
        @Override
        public void onAudioFocusChange(int focusChange) {
            if (focusChange == AudioManager.AUDIOFOCUS_LOSS) {
                // Stop playback
                Log.e(TAG, "AudioManager.AUDIOFOCUS_LOSS");
//                am.unregisterMediaButtonEventReceiver(RemoteControlReceiver);
//                am.abandonAudioFocus(audioFocusChangeListener);
            }else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT){
                // Pause playback
                Log.e(TAG, "AudioManager.AUDIOFOCUS_LOSS_TRANSIENT");
            }else if (focusChange == AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK) {
                // Lower the volume
                Log.e(TAG, "AudioManager.AUDIOFOCUS_LOSS_TRANSIENT_CAN_DUCK");
            } else if (focusChange == AudioManager.AUDIOFOCUS_GAIN) {
                // Resume playback
                Log.e(TAG, "AudioManager.AUDIOFOCUS_GAIN");
            }
        }
    };
}
