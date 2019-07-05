package cn.freeeditor.android;

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


/**
 * Created by kly on 16/8/26.
 */
public class PublisherActivity extends Activity {

    private static final String TAG = "PublisherActivity";

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


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);

        setContentView(R.layout.activity_publisher);

        surfaceView = (SurfaceView)findViewById(R.id.publish_surface);
        overlayView = findViewById(R.id.publish_overlay);
        mUrlEdit = (EditText)findViewById(R.id.publish_edit_url);
        recordButton = (Button)findViewById(R.id.publish_start_button);
        recordButton.setOnClickListener(publishClickListener);
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

        fadeIn();
    }


    @Override
    protected void onDestroy() {
        super.onDestroy();
        closeRecorder();
    }


    private void openRecorder(String url) throws Exception {

    }


    private void closeRecorder(){

    }


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
            if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE){
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
            }else if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_PORTRAIT){
                setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            }
        }
    };


    View.OnClickListener quitClickListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            finish();
        }
    };


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


    private final Handler publishHandler = new PublishHandler(this);

    private static class PublishHandler extends Handler {

        private WeakReference<PublisherActivity> publishReference = null;

        public PublishHandler(PublisherActivity activity) {
            publishReference = new WeakReference<PublisherActivity>(activity);
        }

        @Override
        public void handleMessage(Message msg) {
            PublisherActivity activity = publishReference.get();
            if (activity == null)
                return;

            switch (msg.what) {
                case OVERLAY_FADE_OUT:
                    activity.fadeOut(false);
                    break;
                default:
                    break;
            }
        }
    };
}
