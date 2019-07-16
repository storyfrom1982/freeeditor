package cn.freeeditor.sdk;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

public class VideoCamera implements IVideoSource, Runnable,
        Camera.PreviewCallback, Camera.ErrorCallback {

    private static final String TAG = "VideoCamera";

    private final Thread mMsgThread;
    private final MsgHandler mMsgHandler;

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



    public VideoCamera(){

        mMsgHandler = new MsgHandler(new MsgHandler.IMsgListener() {
            @Override
            public void onMessage(Msg msg) {
                mThreadHandler.sendMessage(mThreadHandler.obtainMessage(MSG_HandleMessage, msg));
            }

            @Override
            public Msg onRequest(Msg msg) {
                return null;
            }
        });

        mMsgThread = new Thread(this);
        mMsgThread.start();

        if (!isRunning.get()){
            synchronized (isRunning){
                try {
                    isRunning.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    @Override
    public void remove(){
        mThreadHandler.sendEmptyMessage(MSG_HandleRemove);
        try {
            mMsgThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    @Override
    public long getInstance(){
        return mMsgHandler.getInstance();
    }


    private void openCamera(Msg msg){

        try {
            mConfig = new JSONObject(msg.msgToString());
            mFrameRate = mConfig.getInt("fps");
            mRequestWidth = mConfig.getInt("width");
            mRequestHeight = mConfig.getInt("height");
            mCameraPosition = mConfig.getString("position");
        } catch (JSONException e) {
            e.printStackTrace();
        }

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

        fixedOutputRotation(90);
        fixedOutputSize(mCamera.getParameters(), mRequestWidth, mRequestHeight);

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

        JSONObject newConfig = new JSONObject();
        try {
            newConfig.put("width", mOutputWidth).put("height", mOutputHeight)
                    .put("croppedWidth", mCroppedWidth).put("croppedHeight", mCroppedHeight)
                    .put("format", 0).put("rotate", 90);
            mMsgHandler.requestMessage(new Msg(MsgKey.Video_Source_FinalConfig, newConfig.toString()));
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }


    private void closeCamera(){
        if (mCamera != null){
            stopCapture();
            mCamera.setErrorCallback(null);
            mCamera.setPreviewCallback(null);
            mCamera.release();
            mCamera = null;
        }
    }


    private void startCapture(){
        Log.d(TAG, "startCapture: enter");
        if (mCamera != null){
            mCamera.startPreview();
            mCamera.setPreviewCallbackWithBuffer(this);
            for (int i = 0; i < 4; ++i){
                mCamera.addCallbackBuffer(new byte[(mOutputWidth * mOutputHeight * 3) >> 1]);
            }
        }
        Log.d(TAG, "startCapture: exit");
    }


    private void stopCapture(){
        if (mCamera != null){
            mCamera.setPreviewCallbackWithBuffer(null);
            mCamera.stopPreview();
        }
    }


    private void internalRemove(){
        mMsgHandler.remove();
        Looper.myLooper().quit();
    }


    private void sendVideoFrame(byte[] frame){
        Msg msg = new Msg(0, frame, 0);
        mMsgHandler.sendMessage(msg);
    }

    private void handleMessage(Msg msg){

        switch (msg.key){
            case MsgKey.Video_Source_Open:
                openCamera(msg);
                break;
            case MsgKey.Video_Source_Close:
                closeCamera();
                break;
            case MsgKey.Video_Source_StartCapture:
                startCapture();
                break;
            case MsgKey.Video_Source_StopCapture:
                stopCapture();
                break;
            default:
                break;
        }
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


    private static final int MSG_HandleVideoFrame = 0;
    private static final int MSG_HandleMessage = 1;
    private static final int MSG_HandleRemove = 2;

    private MessageHandler mThreadHandler;

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
//        Log.e(TAG, "onPreviewFrame: data size: " + data.length);
//        mMsgHandler.requestMessage(new Msg(MsgKey.Video_Source_ProvideFrame, data, 0));
        mCamera.addCallbackBuffer(data);
    }

    @Override
    public void onError(int error, Camera camera) {
        Log.d(TAG, "onError: " + error);
    }


    private static final class MessageHandler extends Handler {

        final WeakReference<VideoCamera> weakReference;

        MessageHandler(VideoCamera deviceCamera){
            weakReference = new WeakReference<>(deviceCamera);
        }

        @Override
        public void handleMessage(Message msg) {
            VideoCamera deviceCamera = weakReference.get();
            if (deviceCamera != null){
                switch (msg.what){
                    case MSG_HandleVideoFrame:
                        deviceCamera.sendVideoFrame((byte[]) msg.obj);
                        break;
                    case MSG_HandleRemove:
                        deviceCamera.internalRemove();
                        break;
                    case MSG_HandleMessage:
                    default:
                        deviceCamera.handleMessage((Msg) msg.obj);
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

            int difference = size.width * size.height - closestWidth * closestHeight;
            if (difference < minimumDifference) {
                minimumDifference = difference;
                isCropped = isCrop;
                mCroppedWidth = closestWidth;
                mCroppedHeight = closestHeight;
                mOutputWidth = size.width;
                mOutputHeight = size.height;
                Log.d(TAG, "fixedOutputSize: output size: " + mOutputWidth + "x" + mOutputHeight
                        + " is crop: " + isCropped + " cropped size: " + mCroppedWidth + "x" + mCroppedHeight);
            }
        }
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
