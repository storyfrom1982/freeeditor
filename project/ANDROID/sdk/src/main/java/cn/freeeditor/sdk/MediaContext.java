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

    private static final int GetMsg_CrateRecorder = 1;
    private static final int GetMsg_GetRecorderConfig = 2;

    private static final int OnGetMsg_CreateCamera = 1;
    private static final int OnGetMsg_CreateMicrophone = 2;


    private int currentOrientation;

    private VideoCamera camera;
    private Microphone microphone;

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
        if (camera != null){
            camera.release();
            camera = null;
        }
        super.release();
        deleteContext(mediaContext);
        mediaContext = 0;
    }

    public long createRecorder(){
        return getLong(GetMsg_CrateRecorder);
    }

    public String getRecorderConfig(){
        return getJson(GetMsg_GetRecorderConfig);
    }

    @Override
    protected void onPutMessage(JNIMessage msg) {

    }

    @Override
    protected JNIMessage onGetMessage(int key) {
        switch (key){
            case OnGetMsg_CreateCamera:
                return new JNIMessage(key, createCamera());
            case OnGetMsg_CreateMicrophone:
                return new JNIMessage(key, createMicrophone());
            default:
                break;
        }
        return new JNIMessage(0);
    }

    private MediaContext(){
        mediaContext = createContext();
        connectContext(mediaContext);
    }

    private long createCamera(){
        if (camera == null){
            camera = new VideoCamera();
        }
        return camera.getJniContext();
    }

    private long createMicrophone(){
        if (microphone == null){
            microphone = new Microphone();
        }
        return microphone.getJniContext();
    }

    private long mediaContext;

    private native long createContext();

    public native void deleteContext(long mediaContext);

    public native void debug();
}
