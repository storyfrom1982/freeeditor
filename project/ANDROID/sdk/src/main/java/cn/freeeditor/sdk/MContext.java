package cn.freeeditor.sdk;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.ComponentCallbacks;
import android.content.Context;
import android.content.res.Configuration;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.atomic.AtomicBoolean;

import cn.freeeditor.sdk.permission.PermissionEverywhere;
import cn.freeeditor.sdk.permission.PermissionResponse;
import cn.freeeditor.sdk.permission.PermissionResultCallback;

public class MContext implements Runnable, ComponentCallbacks {

    public static final String TAG = "MContext";

    private static final int HANDLER_MESSAGE = 0;
    private static final int HANDLER_REMOVE_CONTEXT = 1;
    private static final int HANDLER_PERMISSION_GAN = 3721;
    private static final int HANDLER_PERMISSION_OBTAIN = 3722;

    private final Thread mThread;

    private final MsgHandler mMsgHandler;

    private final WeakReference<Context> mAppCtxReference;

    private final AtomicBoolean isRunning = new AtomicBoolean(false);

    private static MContext mGlobalUniqueCtx = null;

    private int mOrientation = 0;

    public static void Apply(Context context) {
        if (mGlobalUniqueCtx == null){
            mGlobalUniqueCtx = new MContext(context);
        }
    }

    public static MContext Instance(){
        if (mGlobalUniqueCtx == null){
            mGlobalUniqueCtx = new MContext(null);
        }
        return mGlobalUniqueCtx;
    }

    @SuppressLint("UseSparseArrays")
    private MContext(Context context) {

        mAppCtxReference = new WeakReference<Context>(context);

        if (mAppCtxReference.get() != null){
            mAppCtxReference.get().registerComponentCallbacks(this);
            mOrientation = mAppCtxReference.get().getResources().getConfiguration().orientation;
        }

        mListeners = new HashMap<Integer, ArrayList<MsgHandler.IMsgListener>>();

        mMsgHandler = new MsgHandler(new MsgHandler.IMsgListener() {
            @Override
            public void onReceiveMessage(Msg msg) {
                mHandler.sendMessage(mHandler.obtainMessage(HANDLER_MESSAGE, msg));
            }

            @Override
            public Msg onReceiveRequest(Msg msg) {
                synchronized (mListeners){
                    ArrayList<MsgHandler.IMsgListener> list = mListeners.get(msg.key);
                    if (list == null){
                        return new Msg(MsgKey.Null);
                    }
                    return list.get(0).onReceiveRequest(msg);
                }
            }
        });

        registerMessageListener(MsgKey.EnvCtx_HomePath, new MsgHandler.IMsgListener() {
            @Override
            public void onReceiveMessage(Msg msg) {

            }

            @Override
            public Msg onReceiveRequest(Msg msg) {
                Context context = mAppCtxReference.get();
                if (context != null){
                    File configPath = new File(context.getFilesDir().getAbsolutePath() + "/freertc");
                    if (configPath.exists() || configPath.mkdirs()) {
                        return new Msg(MsgKey.OK, configPath.getAbsolutePath());
                    }
                }
                return new Msg(MsgKey.Null);
            }
        });

        mThread = new Thread(this);

        mThread.start();

        if (!isRunning.get()){
            synchronized (isRunning){
                try {
                    isRunning.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }

        apply(mMsgHandler.getInstance());
    }

    public void remove(){
        mHandler.sendEmptyMessage(HANDLER_REMOVE_CONTEXT);
        try {
            mThread.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        debug();
    }

    public void getPermissions(){
        mHandler.sendEmptyMessage(HANDLER_PERMISSION_GAN);
    }

    public IEditor createRecorder(String name){
        Editor recorder = null;
        if (name == null || name.isEmpty()){
            name = "DefaultRecorder";
        }
        Msg msg = requestMessage(new Msg(MsgKey.EnvCtx_CreateCapture, name));
        if (msg.isResultInstance()){
            recorder = new Editor(msg.i64);
        }
        return recorder;
    }

    public void removeRecorder(IEditor irecorder){
        Editor recorder = (Editor) irecorder;
        requestMessage(new Msg(MsgKey.EnvCtx_RemoveCapture, recorder.getNativeInstance()));
        recorder.remove();
    }

//    public String loadConfig(){
//        return sendRequest(new Msg(MSG_KEY_RequestLoadConfig)).troubledPtr;
//    }
//
//    public String saveConfig(){
//        return sendRequest(new Msg(MSG_KEY_RequestSaveConfig)).troubledPtr;
//    }
//
//    public void updateConfig(String cfg){
//        sendRequest(new Msg(MSG_KEY_RequestUpdateConfig, cfg));
//    }

    public Context getApplicationContext(){
        return mAppCtxReference.get();
    }

    public void sendMessage(Msg msg){
        mMsgHandler.sendMessage(msg);
    }

    public Msg requestMessage(Msg msg){
        return mMsgHandler.sendRequest(msg);
    }

    private final HashMap<Integer, ArrayList<MsgHandler.IMsgListener>> mListeners;

    public void registerMessageListener(Integer key, MsgHandler.IMsgListener listener){
        synchronized (mListeners){
            ArrayList<MsgHandler.IMsgListener> list = mListeners.get(key);
            if (list == null){
                list = new ArrayList<MsgHandler.IMsgListener>();
                mListeners.put(key, list);
            }
            list.add(listener);
        }
    }


    private void notifyMessage(Msg msg){
        synchronized (mListeners){
            ArrayList<MsgHandler.IMsgListener> list = mListeners.get(msg.key);
            if (list != null){
                for (int i = 0; i < list.size(); ++i){
                    list.get(i).onReceiveMessage(msg);
                }
            }
        }
    }

    private void setStorageDirPath(){
        File homePath = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/freertc");
        if (homePath.exists() || homePath.mkdirs()) {
            sendMessage(new Msg(MsgKey.EnvCtx_StoragePath, homePath.getAbsolutePath()));
        }
    }


    private int mPermissionDeniedCount = 0;
    private void getWriteFilePermissions(){
        Context context = mAppCtxReference.get();
        if (context != null){
            PermissionEverywhere.getPermission(context,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    HANDLER_PERMISSION_GAN).enqueue(new PermissionResultCallback() {
                @Override
                public void onComplete(PermissionResponse permissionResponse) {
                    if (permissionResponse.getRequestCode() == HANDLER_PERMISSION_GAN){
                        if (permissionResponse.isGranted()) {
                            mHandler.sendEmptyMessage(HANDLER_PERMISSION_OBTAIN);
                        } else {
                            Log.e(TAG, "Get Manifest.permission.WRITE_EXTERNAL_STORAGE failed");
                            if (mPermissionDeniedCount++ < 3){
                                mHandler.sendEmptyMessage(HANDLER_PERMISSION_GAN);
                            }
                        }
                    }
                }
            });
        }
    }

    private void removeContext(){
        if (mAppCtxReference.get() != null){
            mAppCtxReference.get().unregisterComponentCallbacks(this);
        }
        if (mGlobalUniqueCtx != null){
            remove(mMsgHandler.getInstance());
            mMsgHandler.remove();
            mListeners.clear();
            mAppCtxReference.clear();
            mGlobalUniqueCtx = null;
            Looper.myLooper().quit();
        }
    }

    @Override
    public void run() {
        Looper.prepare();
        mHandler = new ContextHandler(this);
        synchronized (isRunning){
            isRunning.set(true);
            isRunning.notifyAll();
        }
        Looper.loop();
    }


    private ContextHandler mHandler;

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        Log.d(TAG, "onConfigurationChanged ======= " + newConfig.orientation);
    }

    @Override
    public void onLowMemory() {

    }

    private static final class ContextHandler extends Handler {

        private final WeakReference<MContext> weakReference;

        ContextHandler(MContext context){
            weakReference = new WeakReference<>(context);
        }

        @Override
        public void handleMessage(Message msg) {
            MContext context = weakReference.get();
            if (context != null){
                switch (msg.what){
                    case HANDLER_REMOVE_CONTEXT:
                        context.removeContext();
                        break;
                    case HANDLER_PERMISSION_GAN:
                        context.getWriteFilePermissions();
                        break;
                    case HANDLER_PERMISSION_OBTAIN:
                        context.setStorageDirPath();
                        break;
                    default:
                        context.notifyMessage((Msg) msg.obj);
                }
            }
        }
    }

    private native void apply(long msgHandler);

    private native void remove(long msgHandler);

    private native void debug();
}
