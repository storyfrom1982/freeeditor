package cn.freeeditor.android;

import android.Manifest;
import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;

import java.lang.ref.WeakReference;

import cn.freeeditor.sdk.MediaContext;
import cn.freeeditor.sdk.MediaRecorder;
import cn.freeeditor.sdk.permission.PermissionEverywhere;
import cn.freeeditor.sdk.permission.PermissionResponse;
import cn.freeeditor.sdk.permission.PermissionResultCallback;


/**
 * Created by kly on 16/8/26.
 */
public class RecordActivity extends Activity {

    private static final String TAG = "RecordActivity";

    private static final int OVERLAY_FADEOUT_TIME = 40000;

    private boolean isShowing = false;
    private boolean isPublishing = false;

    private View overlayView;
    private SurfaceView surfaceView;
    private EditText mUrlEdit;
    private Button recordButton;
    private Button swapCameraButton;
    private Button swapOrientationButton;
    private Button quitButton;

    private MediaRecorder recorder;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_editor);

        surfaceView = (SurfaceView)findViewById(R.id.publish_surface);
        overlayView = findViewById(R.id.publish_overlay);
        mUrlEdit = (EditText)findViewById(R.id.publish_edit_url);
        recordButton = (Button)findViewById(R.id.publish_start_button);
//        recordButton.setOnClickListener(publishClickListener);
        swapCameraButton = (Button)findViewById(R.id.publish_swap_camera);
        swapCameraButton.setOnClickListener(swapCameraClickListener);
        swapOrientationButton = (Button)findViewById(R.id.publish_change_aspect_ratio);
        swapOrientationButton.setOnClickListener(aspectRatioClickListener);
        quitButton = (Button)findViewById(R.id.publish_quit_button);
        quitButton.setOnClickListener(quitClickListener);

        getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
                        | View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
                        | View.SYSTEM_UI_FLAG_IMMERSIVE);

        publishHandler.sendEmptyMessage(HANDLER_PERMISSION_GAN);

        MediaContext.Instance().setAppContext(getApplicationContext());

        fadeIn();

        openRecorder(getUrl());
    }


    private String getUrl(){
        String url;
        if (mUrlEdit.getText().toString().isEmpty()){
            url = mUrlEdit.getHint().toString();
        }else{
            url = mUrlEdit.getHint().toString();
        }

        Log.d(TAG, "URL == " + url);

        if (isPublishing){
            isPublishing = !isPublishing;
//                recordButton.setEnabled(false);
            recordButton.setText("Start");
        }else{
            isPublishing = !isPublishing;
//                recordButton.setEnabled(false);
            recordButton.setText("Stop");
            try {
                url = Environment.getExternalStorageDirectory() + "/" + url;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return url;
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        closeRecorder();
    }


    private void openRecorder(String url){
        recorder = new MediaRecorder();
        recorder.startCapture();
        recorder.startRecord(url);
        recorder.startPreview(surfaceView);
    }


    private void closeRecorder(){
        recorder.stopRecord();
        recorder.stopCapture();
        recorder.release();
        MediaContext.Instance().release();
        MediaContext.debug();
    }

    private void changeOrientation(){
        if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE){
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        }else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT){
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }
        publishHandler.sendEmptyMessageDelayed(HANDLER_ORENTATION, 2000);
    }

//    @Override
//    public void onConfigurationChanged(Configuration newConfig) {
//
//    }

    View.OnClickListener publishClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            final String editText;

            if (mUrlEdit.getText().toString().isEmpty()){
                editText = mUrlEdit.getHint().toString();
            }else{
                editText = mUrlEdit.getHint().toString();
            }

            Log.d(TAG, "URL == " + editText);

            if (isPublishing){
                isPublishing = !isPublishing;
//                recordButton.setEnabled(false);
                recordButton.setText("Start");
            }else{
                isPublishing = !isPublishing;
//                recordButton.setEnabled(false);
                recordButton.setText("Stop");
                try {
                    String url = Environment.getExternalStorageDirectory() + "/" + editText;
                    openRecorder(url);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }
    };


    View.OnClickListener swapCameraClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

        }
    };


    View.OnClickListener aspectRatioClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            publishHandler.sendMessageDelayed(publishHandler.obtainMessage(HANDLER_ORENTATION), 1000);
//            if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE){
//                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
//            }else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT){
//                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
//            }
        }
    };


    View.OnClickListener quitClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            finish();
        }
    };


    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_UP){
            if (!isShowing){
                fadeIn();
            }else{
                fadeOut(true);
            }
        }
        return false;
    }


    private void publisherRunning(){
        recordButton.setText("Stop");
        recordButton.setEnabled(true);
    }


    private static final int OVERLAY_FADE_OUT = 0;
    private static final int HANDLER_PERMISSION_GAN = 3721;
    private static final int HANDLER_ORENTATION = 3722;


    private final Handler publishHandler = new PublishHandler(this);

    private static class PublishHandler extends Handler {

        private WeakReference<RecordActivity> publishReference = null;

        public PublishHandler(RecordActivity activity) {
            publishReference = new WeakReference<RecordActivity>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            RecordActivity activity = publishReference.get();
            if (activity == null)
                return;

            switch (msg.what) {
                case OVERLAY_FADE_OUT:
                    activity.fadeOut(false);
                    break;
                case HANDLER_PERMISSION_GAN:
                    activity.requirePermission();
                    break;
                case HANDLER_ORENTATION:
                    activity.changeOrientation();
                    break;
                default:
                    break;
            }
        }
    }

    private void requirePermission(){
        PermissionEverywhere.getPermission(getApplicationContext(),
                new String[]{
                        Manifest.permission.CAMERA,
                        Manifest.permission.RECORD_AUDIO,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE},
                3721).enqueue(new PermissionResultCallback() {
            @Override
            public void onComplete(PermissionResponse permissionResponse) {
                if (permissionResponse.getRequestCode() == 3721){
                    if (permissionResponse.isGranted()) {

                    } else {
                        cn.freeeditor.sdk.Log.e(TAG, "Get Manifest.permission.WRITE_EXTERNAL_STORAGE failed");
                        publishHandler.sendEmptyMessage(HANDLER_PERMISSION_GAN);
                    }
                }
            }
        });
    }

    /**
     * hider overlay
     */
    private void fadeOut(boolean fromUser) {
        if (isShowing) {
            isShowing = false;
            overlayView.startAnimation(AnimationUtils.loadAnimation(this,
                    android.R.anim.fade_out));
            overlayView.setVisibility(View.INVISIBLE);
        }
    }

    /**
     * show overlay the the default timeout
     */
    private void fadeIn() {
        fadeIn(OVERLAY_FADEOUT_TIME);
    }

    /**
     * show overlay
     */
    private void fadeIn(int timeout) {
        if (!isShowing) {
            isShowing = true;
            overlayView.setVisibility(View.VISIBLE);
        }
        Message msg = publishHandler.obtainMessage(OVERLAY_FADE_OUT);
        if (timeout != 0) {
            publishHandler.removeMessages(OVERLAY_FADE_OUT);
            publishHandler.sendMessageDelayed(msg, timeout);
        }
    }

}
