package cn.freeeditor.sdk;

import android.content.ComponentCallbacks;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.util.DisplayMetrics;
import android.view.Surface;
import android.view.WindowManager;

import java.lang.ref.WeakReference;

public class MediaContext extends JNIContext {

    private static final String TAG = "MediaContext";

    private static final int ReqMsg_ConnectRecorder = 1;
    private static final int ReqMsg_ConnectPlayer = 2;
    private static final int ReqMsg_GetRecorderConfig = 3;

    private static final int SendMsg_DisconnectRecorder = 1;
    private static final int SendMsg_DisconnectPlayer = 2;

    private static final int OnReqMsg_ConnectCamera = 1;
    private static final int OnReqMsg_ConnectMicrophone = 2;

    private static final int OnRecvMsg_DisconnectCamera = 1;
    private static final int OnRecvMsg_DisconnectMicrophone = 2;

    private int currentOrientation;

    private VideoSource videoSource;
    private AudioSource audioSource;

    private static MediaContext sMediaContext = null;

    public static MediaContext Instance(){
        if (sMediaContext == null){
            sMediaContext = new MediaContext();
        }
        return sMediaContext;
    }

    private WeakReference<Context> mAppCtxReference = null;

    public void setAppContext(Context context){
        mAppCtxReference = new WeakReference<>(context);
        currentOrientation = mAppCtxReference.get().getResources().getConfiguration().orientation;
        mAppCtxReference.get().registerComponentCallbacks(new ComponentCallbacks() {
            @Override
            public void onConfigurationChanged(Configuration newConfig) {
                Log.d("MediaContext", "onConfigurationChanged " + newConfig.orientation);
                currentOrientation = newConfig.orientation;
            }

            @Override
            public void onLowMemory() {

            }
        });
    }

    public Context getAppContext(){
        return mAppCtxReference.get();
    }

    public int getOrientation(){
        return currentOrientation;
    }

    public int getScreenOrientation() {
        WindowManager windowManager = ((WindowManager) mAppCtxReference.get().getSystemService(Context.WINDOW_SERVICE));
        int rotation = windowManager.getDefaultDisplay().getRotation();
        DisplayMetrics dm = new DisplayMetrics();
        windowManager.getDefaultDisplay().getMetrics(dm);
        int width = dm.widthPixels;
        int height = dm.heightPixels;
        int orientation;
        // if the device's natural orientation is portrait:
        if ((rotation == Surface.ROTATION_0
                || rotation == Surface.ROTATION_180) && height > width ||
                (rotation == Surface.ROTATION_90
                        || rotation == Surface.ROTATION_270) && width > height) {
            switch(rotation) {
                case Surface.ROTATION_0:
                    orientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
                    break;
                case Surface.ROTATION_90:
                    orientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
                    break;
                case Surface.ROTATION_180:
                    orientation =
                            ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
                    break;
                case Surface.ROTATION_270:
                    orientation =
                            ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
                    break;
                default:
                    Log.e(TAG, "Unknown screen orientation. Defaulting to " +
                            "portrait.");
                    orientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
                    break;
            }
        }
        // if the device's natural orientation is landscape or if the device
        // is square:
        else {
            switch(rotation) {
                case Surface.ROTATION_0:
                    orientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
                    break;
                case Surface.ROTATION_90:
                    orientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
                    break;
                case Surface.ROTATION_180:
                    orientation =
                            ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
                    break;
                case Surface.ROTATION_270:
                    orientation =
                            ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
                    break;
                default:
                    Log.e(TAG, "Unknown screen orientation. Defaulting to " +
                            "landscape.");
                    orientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
                    break;
            }
        }

        return orientation;
    }

    public int getScreenRotation(){
        int degrees = 0;
        WindowManager windowManager = ((WindowManager) mAppCtxReference.get().getSystemService(Context.WINDOW_SERVICE));
        int rotation = windowManager.getDefaultDisplay().getRotation();
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }
        return degrees;
    }

    public void release(){
        disconnectContext();
        super.release();
        if (videoSource != null){
            videoSource.release();
            videoSource = null;
        }
        if (audioSource != null){
            audioSource.release();
            audioSource = null;
        }
        sMediaContext = null;
        disconnectMediaContext();
    }

    public long connectRecorder(){
        JNIMessage msg = requestMessage(ReqMsg_ConnectRecorder);
        return msg.ptr;
    }

    public void disconnectRecorder(long context){
        sendMessage(SendMsg_DisconnectRecorder, context);
    }

    public long connectPlayer(){
        JNIMessage msg = requestMessage(ReqMsg_ConnectPlayer);
        return msg.ptr;
    }

    public void disconnectPlayer(long context){
        sendMessage(SendMsg_DisconnectPlayer, context);
    }

    public String getRecorderConfig(){
        JNIMessage msg = requestMessage(ReqMsg_GetRecorderConfig);
        return msg.string;
    }

    @Override
    protected JNIMessage onObtainMessage(int key) {
        switch (key){
            case OnReqMsg_ConnectCamera:
                return new JNIMessage(key, createCamera());
            case OnReqMsg_ConnectMicrophone:
                return new JNIMessage(key, createMicrophone());
            default:
                break;
        }
        return new JNIMessage();
    }

    @Override
    protected void onRecvMessage(JNIMessage msg) {
        switch (msg.key){
            case OnRecvMsg_DisconnectCamera:
                if (videoSource != null){
                    videoSource.release();
                    videoSource = null;
                }
                break;
            case OnRecvMsg_DisconnectMicrophone:
                if (audioSource != null){
                    audioSource.release();
                    audioSource = null;
                }
                break;
            default:
                break;
        }
    }

    private long createCamera(){
        if (videoSource == null){
            videoSource = new VideoSource();
        }
        return videoSource.getContextPointer();
    }

    private long createMicrophone(){
        if (audioSource == null){
            audioSource = new AudioSource();
        }
        return audioSource.getContextPointer();
    }

    private MediaContext(){
        connectMediaContext(getContextPointer());
    }

    public static native void debug();

    private native void connectMediaContext(long contextPointer);
    private native void disconnectMediaContext();

}
