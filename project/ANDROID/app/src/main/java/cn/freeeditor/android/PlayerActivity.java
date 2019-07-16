package cn.freeeditor.android;

import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.SeekBar;
import android.widget.TextView;

import java.lang.ref.WeakReference;



public class PlayerActivity extends AppCompatActivity {

    private static final String TAG = "PlayerActivity";

    private static final int OVERLAY_FADEOUT_TIME = 40000;

    private boolean isSeeking = false;
    private boolean isShowing = false;
    private boolean isPlaying = false;
    private boolean isPausing = false;

    private int maxDuration;

    private SeekBar seekBar;
    private TextView durationView;
    private TextView currentTimeView;

    private View playOverlayView;
    private View seekBarOverlayView;
    private EditText mUrlEdit;
    private Button stateButton;
    private Button aspectRatioButton;
    private Button quitButton;
    private SurfaceView playSurface;
    private FrameLayout frameLayout;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_player);

        playSurface = (SurfaceView) findViewById(R.id.player_surface);
        playSurface.getHolder().addCallback(callback);
        frameLayout = (FrameLayout) findViewById(R.id.player_surface_frame);

        mUrlEdit = (EditText)findViewById(R.id.play_edit_url);

        seekBar = (SeekBar) findViewById(R.id.player_overlay_seekbar);
        seekBar.setOnSeekBarChangeListener(mSeekListener);

        stateButton = (Button) findViewById(R.id.play_start_button);
        stateButton.setOnClickListener(mPlayStateListener);
        aspectRatioButton = (Button)findViewById(R.id.play_change_aspect_ratio);
        aspectRatioButton.setOnClickListener(aspectRatioClickListener);
        quitButton = (Button)findViewById(R.id.play_quit_button);
        quitButton.setOnClickListener(quitClickListener);

        playOverlayView = findViewById(R.id.play_overlay);
        seekBarOverlayView = findViewById(R.id.seekbar_overlay);

        currentTimeView = (TextView) findViewById(R.id.player_overlay_time);
        durationView = (TextView) findViewById(R.id.player_overlay_length);

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


    private final View.OnClickListener mPlayStateListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            String url = null;

            if (mUrlEdit.getText().toString().isEmpty()){
                url = Environment.getExternalStorageDirectory() + "/" + mUrlEdit.getHint().toString();
            }else{
                url = Environment.getExternalStorageDirectory() + "/" + mUrlEdit.getHint().toString();
            }

            Log.i(TAG, "URL == " + url);
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


    private final SeekBar.OnSeekBarChangeListener mSeekListener = new SeekBar.OnSeekBarChangeListener() {

        private float seekPosition = 0;
        private boolean progressChanged = false;

        @Override
        public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
            if (fromUser) {
                progressChanged = true;
                seekPosition = (float) progress / maxDuration;
            }
        }

        @Override
        public void onStartTrackingTouch(SeekBar seekBar) {
            if (isPlaying) {
                seekPosition = 0;
                isSeeking = true;
                fadeIn();
            }
        }

        @Override
        public void onStopTrackingTouch(SeekBar seekBar) {
            if (isPlaying && progressChanged) {
                progressChanged = false;
            }
        }
    };


    private static final int OVERLAY_FADE_OUT = 0;


    private final Handler playHandler = new playerHandler(this);


    private static class playerHandler extends Handler {

        private WeakReference<PlayerActivity> playReference = null;

        public playerHandler(PlayerActivity owner) {
            playReference = new WeakReference<PlayerActivity>(owner);
        }

        @Override
        public void handleMessage(Message msg) {
            PlayerActivity activity = playReference.get();
            if (activity == null)
                return;

            switch (msg.what) {
                case OVERLAY_FADE_OUT:
                    activity.fadeOut(false);
                default:
                    Log.d(TAG, "Message type ==== " + msg.what);
                    break;
            }
        }
    }

    /**
     * hider overlay
     */
    private void fadeOut(boolean fromUser) {
        if (isShowing) {
            isShowing = false;
            playOverlayView.startAnimation(AnimationUtils.loadAnimation(this, android.R.anim.fade_out));
            seekBarOverlayView.startAnimation(AnimationUtils.loadAnimation(this, android.R.anim.fade_out));
            playOverlayView.setVisibility(View.INVISIBLE);
            seekBarOverlayView.setVisibility(View.INVISIBLE);
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
            playOverlayView.setVisibility(View.VISIBLE);
            seekBarOverlayView.setVisibility(View.VISIBLE);
        }
        Message msg = playHandler.obtainMessage(OVERLAY_FADE_OUT);
        if (timeout != 0) {
            playHandler.removeMessages(OVERLAY_FADE_OUT);
            playHandler.sendMessageDelayed(msg, timeout);
        }
    }


    SurfaceHolder.Callback callback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            createRenderer(holder.getSurface());
            rendererDraw();
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            releaseRenderer();
        }
    };


    private native void createRenderer(Surface surface);

    private native void releaseRenderer();

    private native void rendererDraw();

}
