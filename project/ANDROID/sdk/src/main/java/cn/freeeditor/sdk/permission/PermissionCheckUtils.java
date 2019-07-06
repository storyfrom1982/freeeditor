package cn.freeeditor.sdk.permission;

import android.hardware.Camera;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;

import java.lang.reflect.Field;


public class PermissionCheckUtils {

    public static boolean checkAudioPermission() {
        int sampleRate = 44100;
        int minBufferSize;
        AudioRecord audioRecord;
        minBufferSize = AudioRecord.getMinBufferSize(
                sampleRate,
                AudioFormat.CHANNEL_IN_STEREO,
                AudioFormat.ENCODING_PCM_16BIT);
        try {
            audioRecord =  new AudioRecord(
                    MediaRecorder.AudioSource.MIC,
                    sampleRate,
                    AudioFormat.CHANNEL_IN_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    minBufferSize);
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
        if (audioRecord.getState() != AudioRecord.STATE_INITIALIZED) {
            return false;
        }
        try{
            audioRecord.startRecording();
        }catch (IllegalStateException e){
            e.printStackTrace();
        }
        if (audioRecord.getRecordingState() != AudioRecord.RECORDSTATE_RECORDING) {
            return false;
        }
        byte[] sampleBuffer = new byte[minBufferSize];
        minBufferSize = audioRecord.read(sampleBuffer, 0, sampleBuffer.length);
        if (minBufferSize == AudioRecord.ERROR_INVALID_OPERATION || minBufferSize != sampleBuffer.length) {
            return false;
        }
        audioRecord.stop();
        audioRecord.release();
        return true;
    }


    public static boolean checkCameraPermission() {
        Camera mCamera;
        try {
            mCamera = Camera.open();
            if (mCamera == null){
                return false;
            }
            mCamera.getParameters();
            mCamera.release();
        } catch (Exception e) {
            return false;
        }
        return checkVivoCameraPermission(mCamera);
    }

    private static boolean checkVivoCameraPermission(Camera camera){
        boolean hasPermission;
        Field mHasPermissionField;
        try {
            mHasPermissionField = camera.getClass().getDeclaredField("mHasPermission");
            mHasPermissionField.setAccessible(true);
            hasPermission = (boolean) mHasPermissionField.get(camera);
        } catch (Exception e) {
            e.printStackTrace();
            return true;
        }
        return hasPermission;
    }

}
