package cn.freeeditor.sdk;

import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Message;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONObject;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import static android.content.pm.ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
import static android.content.pm.ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;


public class VideoSource extends JNIContext
        implements Camera.PreviewCallback, Camera.ErrorCallback {

    private static final String TAG = "VideoSource";

    private int mStatus;

    private int mDeviceId;
    private int mRotation;
    private int mSrcWidth;
    private int mSrcHeight;
    private int mFinalWidth;
    private int mFinalHeight;

    private Camera mCamera;
    private JSONObject mConfig;
    protected long mStartTime;

    private int mBufferCount = 4;
    private ArrayList<byte[]> mBufferList = new ArrayList<>();
    private final SurfaceTexture mSurfaceTexture = new SurfaceTexture(buildTexture());


    public VideoSource(){
        startHandler(getClass().getName());
        mStatus = MediaStatus.Status_Closed;
    }

    public void release(){
        msgHandler.sendMessage(msgHandler.obtainMessage(MsgKey.Media_Close));
        stopHandler();
    }

    private void openCamera(JNIMessage msg){

        if (mStatus != MediaStatus.Status_Closed){
            Log.e(TAG, "[VideoSource openCamera] error status: " + mStatus);
            return;
        }

        mConfig = JSON.parseObject(msg.string);
        int videoWidth = mConfig.getIntValue(MediaConfig.VIDEO_WIDTH);
        int videoHeight = mConfig.getIntValue(MediaConfig.VIDEO_HEIGHT);
        int videoFrameRate = mConfig.getIntValue(MediaConfig.VIDEO_FRAME_RATE);
        String videoDevice = mConfig.getString(MediaConfig.VIDEO_DEVICE);

        if (videoDevice.equals(MediaConfig.VIDEO_DEVICE_FRONT)){
            mDeviceId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        }else {
            mDeviceId = Camera.CameraInfo.CAMERA_FACING_BACK;
        }

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
                mStatus = MediaStatus.Status_Error;
                JSONObject js = new JSONObject();
                js.put("error", "camera open failed");
                sendMessage(MsgKey.Media_ProcessEvent, mStatus, js.toJSONString());
                return;
            }
        }

        mCamera.setErrorCallback(this);

        fixedRotation(MediaContext.Instance().getScreenRotation());

        if (videoWidth >= videoHeight){
            fixedVideoSize(mCamera.getParameters(), videoWidth, videoHeight);
        }else {
            fixedVideoSize(mCamera.getParameters(), videoHeight, videoWidth);
        }

        Camera.Parameters parameters = mCamera.getParameters();
        int minValue = parameters.getMinExposureCompensation();
        int maxValue = parameters.getMaxExposureCompensation();
        Log.d(TAG, "[VideoSource openCamera] ExposureCompensation: "
                + minValue + "<-->" + maxValue);

        List<String> modes = parameters.getSupportedFocusModes();
        for( int i=0; i<modes.size(); i++){
            String s = modes.get(i);
            Log.d(TAG, "[VideoSource openCamera] SupportedFocusModes: " + s);
            if (s.equals(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)){
                parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
            }
        }

        parameters.setPreviewFormat(ImageFormat.NV21);
        parameters.setPreviewSize(mSrcWidth, mSrcHeight);
        mCamera.setParameters(parameters);
        try {
            mCamera.setPreviewTexture(mSurfaceTexture);
        } catch (IOException e) {
            e.printStackTrace();
            mCamera.release();
            mCamera = null;
            mStatus = MediaStatus.Status_Error;
            JSONObject js = new JSONObject();
            js.put("error", "camera setPreviewTexture failed");
            sendMessage(MsgKey.Media_ProcessEvent, mStatus, js.toJSONString());
            return;
        }

        int imageFormat = mCamera.getParameters().getPreviewFormat();
        if (imageFormat != ImageFormat.NV21 && imageFormat != ImageFormat.YV12){
            mCamera.release();
            mCamera = null;
            mStatus = MediaStatus.Status_Error;
            JSONObject js = new JSONObject();
            js.put("error", "camera unsupported image format: " + imageFormat);
            sendMessage(MsgKey.Media_ProcessEvent, mStatus, js.toJSONString());
            return;
        }

        for (int i = 0; i < mBufferCount; ++i){
            mBufferList.add(new byte[((mSrcWidth * mSrcHeight) >> 1) * 3]);
        }

        if (imageFormat == ImageFormat.NV21){
            mConfig.put(MediaConfig.VIDEO_SRC_IMAGE_FORMAT, MediaConfig.VIDEO_IMAGE_FORMAT_NV21);
        }else {
            mConfig.put(MediaConfig.VIDEO_SRC_IMAGE_FORMAT, MediaConfig.VIDEO_IMAGE_FORMAT_I420);
        }

        mConfig.put(MediaConfig.VIDEO_SRC_WIDTH, mSrcWidth);
        mConfig.put(MediaConfig.VIDEO_SRC_HEIGHT, mSrcHeight);

        int orientation = MediaContext.Instance().getScreenOrientation();

        if (orientation == SCREEN_ORIENTATION_PORTRAIT
                || orientation == SCREEN_ORIENTATION_REVERSE_PORTRAIT){
            mConfig.put(MediaConfig.VIDEO_WIDTH, mFinalHeight);
            mConfig.put(MediaConfig.VIDEO_HEIGHT, mFinalWidth);
        }else {
            mConfig.put(MediaConfig.VIDEO_WIDTH, mFinalWidth);
            mConfig.put(MediaConfig.VIDEO_HEIGHT, mFinalHeight);
        }

        if (mDeviceId == Camera.CameraInfo.CAMERA_FACING_FRONT){
            mConfig.put(MediaConfig.VIDEO_DEVICE, MediaConfig.VIDEO_DEVICE_FRONT);
        }else {
            mConfig.put(MediaConfig.VIDEO_DEVICE, MediaConfig.VIDEO_DEVICE_BACK);
        }

        mConfig.put(MediaConfig.VIDEO_SRC_ROTATION, mRotation);

        mStatus = MediaStatus.Status_Opened;
        sendMessage(MsgKey.Media_ProcessEvent, mStatus, mConfig.toString());

        mStartTime = 0;

    }


    private void closeCamera(){
        if (mStatus == MediaStatus.Status_Started){
            stopCapture();
        }
        if (mStatus == MediaStatus.Status_Opened
                || mStatus == MediaStatus.Status_Stopped){
            if (mCamera != null){
                mCamera.setErrorCallback(null);
                mCamera.release();
                mCamera = null;
                mBufferList.clear();
                mStatus = MediaStatus.Status_Closed;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }


    private void startCapture(){
        if (mStatus == MediaStatus.Status_Opened
                || mStatus == MediaStatus.Status_Stopped){
            if (mCamera != null){
                mCamera.startPreview();
                mCamera.setPreviewCallbackWithBuffer(this);
                mStatus = MediaStatus.Status_Started;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
                for (int i = 0; i < mBufferCount; ++i){
                    mCamera.addCallbackBuffer(mBufferList.get(i));
                }
            }
        }
    }


    private void stopCapture(){
        if (mStatus == MediaStatus.Status_Started){
            if (mCamera != null){
                mCamera.setPreviewCallbackWithBuffer(null);
                mCamera.stopPreview();
                mStatus = MediaStatus.Status_Stopped;
                sendMessage(MsgKey.Media_ProcessEvent, mStatus);
            }
        }
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        sendMessage(MsgKey.Media_ProcessData, data, data.length);
        mCamera.addCallbackBuffer(data);
    }

    @Override
    public void onError(int error, Camera camera) {
        mStatus = MediaStatus.Status_Error;
        JSONObject js = new JSONObject();
        if (error == android.hardware.Camera.CAMERA_ERROR_SERVER_DIED){
            js.put("error", "camera server died");
        }else {
            js.put("error", "camera error unknown");
        }
        sendMessage(MsgKey.Media_ProcessEvent, mStatus, js.toJSONString());
    }

    @Override
    void onFinalRelease() {
        super.release();
    }

    @Override
    void onMessageProcessor(Message msg) {
        switch (msg.what){
            case MsgKey.Media_Open:
                openCamera((JNIMessage) msg.obj);
                break;
            case MsgKey.Media_Start:
                startCapture();
                break;
            case MsgKey.Media_Stop:
                stopCapture();
                break;
            case MsgKey.Media_Close:
                closeCamera();
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

    private void fixedVideoSize(Camera.Parameters parameters, final int width, final int height){
        int difference = Integer.MAX_VALUE;
        List<Camera.Size> sizes = parameters.getSupportedPreviewSizes();
        for (Camera.Size size : sizes) {
            if (size.width >= width && size.height >= height) {
                int d = size.width * size.height - width * height;
                if (difference > d){
                    difference = d;
                    mSrcWidth = size.width;
                    mSrcHeight = size.height;
                    mFinalWidth = width;
                    mFinalHeight = height;
                }
            }
        }
        if (difference == Integer.MAX_VALUE){
            for (Camera.Size size : sizes) {
                float ar = (float) (Math.round(((float) width / height) * 1000.0f)) / 1000.0f;
                float AR = (float) (Math.round(((float) size.width / size.height) * 1000.0f)) / 1000.0f;
                int w = size.width;
                int h = size.height;
                if (ar >= AR){
                    h = (int) (size.width / ar);
                }else {
                    w = (int) (size.height * ar);
                }
                w = (w + 3) & ~3;
                h = (h + 3) & ~3;
                int d = Math.abs(width * height - w * h);
                if (difference > d && size.width >= w && size.height >= h){
                    difference = d;
                    mSrcWidth = size.width;
                    mSrcHeight = size.height;
                    mFinalWidth = w;
                    mFinalHeight = h;
                }
            }
        }
    }


    private void fixedRotation(int rotate){
        Camera.CameraInfo info = new Camera.CameraInfo();
        try {
            Camera.getCameraInfo(mDeviceId, info);
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            mRotation = (info.orientation + rotate) % 360;
        } else { // back-facing
            mRotation = (info.orientation - rotate + 360) % 360;
        }
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
