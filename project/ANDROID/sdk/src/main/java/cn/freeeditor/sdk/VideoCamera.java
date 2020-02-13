package cn.freeeditor.sdk;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

//import org.json.JSONException;
//import org.json.JSONObject;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import static android.content.pm.ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
import static android.content.pm.ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;

public class VideoCamera extends JNIContext implements Runnable,
        Camera.PreviewCallback, Camera.ErrorCallback {

    private static final String TAG = "VideoCamera";

    private final Thread mThread;

    private final AtomicBoolean isRunning = new AtomicBoolean(false);
    private final SurfaceTexture mSurfaceTexture = new SurfaceTexture(buildTexture());


    private Camera mCamera;
    private int mDeviceId;
    private int mRotation;
    private int mRequestWidth;
    private int mRequestHeight;
    private int mOutputWidth;
    private int mOutputHeight;
    private int mCroppedWidth;
    private int mCroppedHeight;
    private boolean isCropped = false;
    private int mFrameRate;
    private String mCameraPosition;
    private JSONObject mConfig;

    private boolean isCapture = false;

    private int bufferCount = 4;
    private ArrayList<byte[]> bufferList = new ArrayList<>();


    public VideoCamera(){
        mThread = new Thread(this);
        mThread.start();
        synchronized (isRunning){
            if (!isRunning.get()){
                try {
                    isRunning.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    public void release(){
        mThreadHandler.sendEmptyMessage(OnPutMsg_Destroy);
        try {
            mThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onPutMessage(JNIMessage msg) {
        mThreadHandler.sendMessage(mThreadHandler.obtainMessage(msg.key, msg));
    }

    @Override
    protected JNIMessage onGetMessage(int key) {
        return new JNIMessage();
    }


    private void openCamera(JNIMessage msg){

        mConfig = JSON.parseObject(msg.json);
        mFrameRate = mConfig.getIntValue("fps");
        mRequestWidth = mConfig.getIntValue("width");
        mRequestHeight = mConfig.getIntValue("height");
        mCameraPosition = mConfig.getString("position");

        if (mCameraPosition.equals("front")){
            mDeviceId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        }else {
            mDeviceId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }

        Log.d(TAG, "openCamera: " + "size: " + mRequestWidth + "x" + mRequestHeight + " fps: " + mFrameRate + " position: " + mCameraPosition);

        try {
            mCamera = Camera.open(mDeviceId);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (mCamera == null){
            if (mDeviceId == Camera.CameraInfo.CAMERA_FACING_BACK){
                mDeviceId = Camera.CameraInfo.CAMERA_FACING_FRONT;
            }else {
                mDeviceId = Camera.CameraInfo.CAMERA_FACING_BACK;
            }
            try {
                mCamera = Camera.open(mDeviceId);
            } catch (Exception e) {
                e.printStackTrace();
                return;
            }
        }

        mCamera.setErrorCallback(this);

        fixedOutputRotation(MediaContext.Instance().getScreenRotation());

        if (mRequestWidth >= mRequestHeight){
            fixedOutputSize(mCamera.getParameters(), mRequestWidth, mRequestHeight);
        }else {
            fixedOutputSize(mCamera.getParameters(), mRequestHeight, mRequestWidth);
        }

        Camera.Parameters parameters = mCamera.getParameters();
        int minValue = parameters.getMinExposureCompensation();
        int maxValue = parameters.getMaxExposureCompensation();
        Log.d(TAG, "openCamera: ExposureCompensation: " + minValue + "<-->" + maxValue);

        List<String> modes = parameters.getSupportedFocusModes();
        for( int i=0; i<modes.size(); i++){
            String s = modes.get(i);
            Log.d(TAG, "openCamera: SupportedFocusModes: " + s);
            if (s.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)){
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }
        }

        parameters.setPreviewSize(mOutputWidth, mOutputHeight);
        mCamera.setParameters(parameters);
        try {
            mCamera.setPreviewTexture(mSurfaceTexture);
        } catch (IOException e) {
            e.printStackTrace();
        }

        JSONObject json = new JSONObject();
        json.put("width", mOutputWidth);
        json.put("height", mOutputHeight);
        int orientation = MediaContext.Instance().getScreenOrientation();
        if (orientation == SCREEN_ORIENTATION_PORTRAIT || orientation == SCREEN_ORIENTATION_REVERSE_PORTRAIT){
            json.put("croppedWidth", mCroppedHeight);
            json.put("croppedHeight", mCroppedWidth);
        }else {
            json.put("croppedWidth", mCroppedWidth);
            json.put("croppedHeight", mCroppedHeight);
        }
        json.put("format", 0);
        json.put("rotate", mRotation);
//            newConfig.put("width", mOutputWidth).put("height", mOutputHeight)
//                    .put("croppedWidth", mCroppedWidth).put("croppedHeight", mCroppedHeight)
//                    .put("format", 0).put("rotate", mRotation);
        putJson(PutMsg_Opened, json.toString());

        for (int i = 0; i < bufferCount; ++i){
            bufferList.add(new byte[(mOutputWidth * mOutputHeight * 3) >> 1]);
        }
    }


    private void closeCamera(){
        if (mCamera != null){
            stopCapture();
            mCamera.setErrorCallback(null);
            mCamera.release();
            mCamera = null;
            putMessage(PutMsg_Closed);
        }
    }


    private void startCapture(){
        Log.d(TAG, "Start: enter");
        if (mCamera != null){
            mCamera.startPreview();
            mCamera.setPreviewCallbackWithBuffer(this);
            for (int i = 0; i < bufferCount; ++i){
                mCamera.addCallbackBuffer(bufferList.get(i));
            }
            isCapture = true;
            putMessage(PutMsg_Started);
        }
        Log.d(TAG, "Start: exit");
    }


    private void stopCapture(){
        if (mCamera != null && isCapture){
            isCapture = false;
            mCamera.setPreviewCallbackWithBuffer(null);
            mCamera.stopPreview();
            putMessage(PutMsg_Stopped);
        }
    }


    private void destroy(){
        Looper.myLooper().quit();
        super.release();
    }

    @Override
    public void run() {
        Looper.prepare();
        mThreadHandler = new MessageHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
    }

    private static final int PutMsg_Opened = 1;
    private static final int PutMsg_Started = 2;
    private static final int PutMsg_Stopped = 3;
    private static final int PutMsg_Closed = 4;
    private static final int PutMsg_ProcessPicture = 5;
    private static final int PutMsg_Destroy = 6;

    private static final int OnPutMsg_Open = 1;
    private static final int OnPutMsg_Start = 2;
    private static final int OnPutMsg_Stop = 3;
    private static final int OnPutMsg_Close = 4;
    private static final int OnPutMsg_Destroy = 5;


    private MessageHandler mThreadHandler;

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        putBuffer(PutMsg_ProcessPicture, data, data.length);
        mCamera.addCallbackBuffer(data);
    }

    @Override
    public void onError(int error, Camera camera) {
        Log.d(TAG, "onError: " + error);
        putJson(0, "" + error);
    }


    private static final class MessageHandler extends Handler {

        final WeakReference<VideoCamera> weakReference;

        MessageHandler(VideoCamera deviceCamera){
            weakReference = new WeakReference<>(deviceCamera);
        }

        @Override
        public void handleMessage(Message msg) {
            VideoCamera videoCamera = weakReference.get();
            if (videoCamera != null){
                switch (msg.what){
                    case OnPutMsg_Open:
                        videoCamera.openCamera((JNIMessage) msg.obj);
                        break;
                    case OnPutMsg_Start:
                        videoCamera.startCapture();
                        break;
                    case OnPutMsg_Stop:
                        videoCamera.stopCapture();
                        break;
                    case OnPutMsg_Close:
                        videoCamera.closeCamera();
                        break;
                    case OnPutMsg_Destroy:
                        videoCamera.destroy();
                        break;
                    default:
                        break;
                }
            }
        }
    }

    private void fixedOutputSize(Camera.Parameters parameters, final int width, final int height){

        Log.d(TAG, "fixedOutputSize: request size : " + width + "x" + height);
        int minimumDifference = Integer.MAX_VALUE;
        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();

        for (Camera.Size size : sizes) {

            Log.d(TAG, "fixedOutputSize: supported output size: " + size.width + "x" + size.height);
            float ar = (float) (Math.round(((float) size.width / size.height) * 1000.0f)) / 1000.0f;
            float aspectRatio = (float) (Math.round(((float) width / height) * 1000.0f)) / 1000.0f;

            boolean isCrop = false;
            int closestWidth = size.width;
            int closestHeight = size.height;

            if (ar != aspectRatio || (closestWidth & 0xf) != 0) {
                isCrop = true;
                int cropWidth = (int) (size.height * aspectRatio);
                int cropHeight = (int) (size.width / aspectRatio);
                cropWidth = ((cropWidth + 15) & ~15);
                cropHeight = (cropHeight + 1) & ~1;
                if (cropWidth <= size.width && size.height >= height){
                    closestWidth = cropWidth;
                    closestHeight = size.height;
                }else if (cropHeight <= size.height && size.width >= width){
                    closestWidth = size.width;
                    closestHeight = cropHeight;
                }else {
                    continue;
                }
            }

            int difference = size.width * size.height - width * height;
            if (difference < minimumDifference) {
                minimumDifference = difference;
                isCropped = isCrop;
                mCroppedWidth = closestWidth;
                mCroppedHeight = closestHeight;
                mOutputWidth = size.width;
                mOutputHeight = size.height;
            }
        }

        Log.d(TAG, "fixedOutputSize: output size: " + mOutputWidth + "x" + mOutputHeight
                + " is crop: " + isCropped + " cropped size: " + mCroppedWidth + "x" + mCroppedHeight);
    }

    private void fixedOutputRotation(int rotate){
        Camera.CameraInfo info = new Camera.CameraInfo();
        try {
            Camera.getCameraInfo(mDeviceId, info);
        } catch (Exception e) {
            Log.e(TAG, "fixedOutputRotation: getCameraInfo exception");
            e.printStackTrace();
        }
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            mRotation = (info.orientation + rotate) % 360;
        } else { // back-facing
            mRotation = (info.orientation - rotate + 360) % 360;
        }
        Log.d(TAG, "fixedOutputRotation: camera orientation: " + info.orientation + " rotate: " + mRotation);
    }


    private int buildTexture(){
        int[] textures = new int[1];
        GLES20.glGenTextures(1, textures, 0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, textures[0]);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_NEAREST);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_CLAMP_TO_EDGE);
        return textures[0];
    }

}
