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

public class VideoSource extends JNIContext implements Runnable,
        Camera.PreviewCallback, Camera.ErrorCallback {

    private static final String TAG = "VideoSource";

    private static final int Status_Closed = 0;
    private static final int Status_Opened = 1;
    private static final int Status_Started = 2;
    private static final int Status_Stopped = 3;

    private int mStatus;

    private int mDeviceId;
    private int mRotation;
    private int mSrcWidth;
    private int mSrcHeight;
    private int mFinalWidth;
    private int mFinalHeight;

    private int mCodecFPS;
    private int mCodecWidth;
    private int mCodecHeight;
    private String mSrcPosition;
    private JSONObject mConfig;

    private Camera mCamera;

    private int mBufferCount = 4;
    private ArrayList<byte[]> mBufferList = new ArrayList<>();

    private final Thread mMessageThread;
    private final AtomicBoolean isRunning = new AtomicBoolean(false);
    private final SurfaceTexture mSurfaceTexture = new SurfaceTexture(buildTexture());


    public VideoSource(){
        mMessageThread = new Thread(this);
        mMessageThread.start();
        synchronized (isRunning){
            if (!isRunning.get()){
                try {
                    isRunning.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        mStatus = Status_Closed;
    }

    public void release(){
        mMessageHandler.sendMessage(mMessageHandler.obtainMessage(RecvMsg_Close));
        mMessageHandler.sendMessage(mMessageHandler.obtainMessage(RecvMsg_Destroy));
        try {
            mMessageThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private void openCamera(JNIMessage msg){

        if (mStatus != Status_Closed){
            return;
        }

        mConfig = JSON.parseObject(msg.string);
        mCodecFPS = mConfig.getIntValue("codecFPS");
        mCodecWidth = mConfig.getIntValue("codecWidth");
        mCodecHeight = mConfig.getIntValue("codecHeight");
        mSrcPosition = mConfig.getString("srcPosition");

        if (mSrcPosition.equals("front")){
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
                return;
            }
        }

        mCamera.setErrorCallback(this);

        fixedOutputRotation(MediaContext.Instance().getScreenRotation());

        if (mCodecWidth >= mCodecHeight){
            fixedOutputSize(mCamera.getParameters(), mCodecWidth, mCodecHeight);
        }else {
            fixedOutputSize(mCamera.getParameters(), mCodecHeight, mCodecWidth);
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

        parameters.setPreviewSize(mSrcWidth, mSrcHeight);
        mCamera.setParameters(parameters);
        try {
            mCamera.setPreviewTexture(mSurfaceTexture);
        } catch (IOException e) {
            e.printStackTrace();
        }

        for (int i = 0; i < mBufferCount; ++i){
            mBufferList.add(new byte[(mSrcWidth * mSrcHeight * 3) >> 1]);
        }

        mConfig.put("srcWidth", mSrcWidth);
        mConfig.put("srcHeight", mSrcHeight);

        int orientation = MediaContext.Instance().getScreenOrientation();
        if (orientation == SCREEN_ORIENTATION_PORTRAIT || orientation == SCREEN_ORIENTATION_REVERSE_PORTRAIT){
            mConfig.put("codecWidth", mFinalHeight);
            mConfig.put("codecHeight", mFinalWidth);
        }else {
            mConfig.put("codecWidth", mFinalWidth);
            mConfig.put("codecHeight", mFinalHeight);
        }
        if (mDeviceId == Camera.CameraInfo.CAMERA_FACING_FRONT){
            mConfig.put("srcPosition", "front");
        }else {
            mConfig.put("srcPosition", "back");
        }
        mConfig.put("srcFormat", "YV21");
        mConfig.put("srcRotation", mRotation);
        sendMessage(new JNIMessage(SendMsg_Opened, mConfig.toString()));

        mStatus = Status_Opened;
    }


    private void closeCamera(){
        if (mStatus == Status_Started){
            stopCapture();
        }
        if (mStatus == Status_Opened || mStatus == Status_Stopped){
            if (mCamera != null){
                mCamera.setErrorCallback(null);
                mCamera.release();
                mCamera = null;
                mBufferList.clear();
                sendMessage(SendMsg_Closed);
                mStatus = Status_Closed;
            }
        }
    }


    private void startCapture(){
        if (mStatus == Status_Opened || mStatus == Status_Stopped){
            if (mCamera != null){
                mCamera.startPreview();
                mCamera.setPreviewCallbackWithBuffer(this);
                for (int i = 0; i < mBufferCount; ++i){
                    mCamera.addCallbackBuffer(mBufferList.get(i));
                }
                sendMessage(SendMsg_Started);
                mStatus = Status_Started;
            }
        }
    }


    private void stopCapture(){
        if (mStatus == Status_Started){
            if (mCamera != null){
                mCamera.setPreviewCallbackWithBuffer(null);
                mCamera.stopPreview();
                sendMessage(SendMsg_Stopped);
                mStatus = Status_Stopped;
            }
        }
    }


    private void destroy(){
        Looper.myLooper().quit();
        super.release();
    }

    @Override
    public void run() {
        Looper.prepare();
        mMessageHandler = new MessageHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        sendMessage(SendMsg_ProcessPicture, data, data.length);
        mCamera.addCallbackBuffer(data);
    }

    @Override
    public void onError(int error, Camera camera) {
        Log.d(TAG, "onError: " + error);
        sendMessage(-1);
    }


    private static final int SendMsg_Opened = 1;
    private static final int SendMsg_Started = 2;
    private static final int SendMsg_Stopped = 3;
    private static final int SendMsg_Closed = 4;
    private static final int SendMsg_ProcessPicture = 5;

    private static final int RecvMsg_Open = 1;
    private static final int RecvMsg_Start = 2;
    private static final int RecvMsg_Stop = 3;
    private static final int RecvMsg_Close = 4;
    private static final int RecvMsg_Destroy = 5;


    private MessageHandler mMessageHandler;

    private static final class MessageHandler extends Handler {

        final WeakReference<VideoSource> weakReference;

        MessageHandler(VideoSource deviceCamera){
            weakReference = new WeakReference<>(deviceCamera);
        }

        @Override
        public void handleMessage(Message msg) {
            VideoSource videoCamera = weakReference.get();
            if (videoCamera != null){
                switch (msg.what){
                    case RecvMsg_Open:
                        videoCamera.openCamera((JNIMessage) msg.obj);
                        break;
                    case RecvMsg_Start:
                        videoCamera.startCapture();
                        break;
                    case RecvMsg_Stop:
                        videoCamera.stopCapture();
                        break;
                    case RecvMsg_Close:
                        videoCamera.closeCamera();
                        break;
                    case RecvMsg_Destroy:
                        videoCamera.destroy();
                        break;
                    default:
                        break;
                }
            }
        }
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        return new JNIMessage();
    }

    @Override
    protected void onReceiveMessage(JNIMessage msg) {
        mMessageHandler.sendMessage(mMessageHandler.obtainMessage(msg.key, msg));
    }

    private void fixedOutputSize(Camera.Parameters parameters, final int width, final int height){

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


    private void fixedOutputRotation(int rotate){
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
