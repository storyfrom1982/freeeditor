package cn.freeeditor.sdk;

import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

import java.nio.ByteBuffer;

public class MediaRecord implements SurfaceHolder.Callback {


    private static final int Record_SetConfig = 0;
    private static final int Record_StartCapture = 1;
    private static final int Record_StartRecord = 2;
    private static final int Record_StopCapture = 3;
    private static final int Record_StopRecord = 4;
    private static final int Record_ChangeCameraConfig = 5;
    private static final int Record_ChangeEncodeConfig = 6;

    private long jniObject;
    private final JNIHandler jniHandler;

    private SurfaceView mVideoView;

    public MediaRecord(){
        jniHandler = new JNIHandler();
        jniHandler.setListener(new JNIHandler.IJNIListener() {
            @Override
            public int onPutObject(int type, long obj) {
                return 0;
            }

            @Override
            public int onPutMessage(int cmd, String msg) {
                Log.e("MediaRecord", msg);
                return 0;
            }

            @Override
            public int onPutData(byte[] data, int size) {
                return 0;
            }

            @Override
            public long onGetObject(int type) {
                return 0;
            }

            @Override
            public String onGetMessage(int cmd) {
                return null;
            }

            @Override
            public ByteBuffer onGetBuffer() {
                return null;
            }
        });

        jniObject = JNIContext.Instance().getObject(JNIContext.Cmd_GetRecord);
        jniHandler.setContext(jniObject);

        String configStr = JNIContext.Instance().getMessage(JNIContext.Cmd_GetRecordConfig);
        jniHandler.putMessage(Record_SetConfig, configStr);
    }

    public void startCapture(){
        jniHandler.putMessage(Record_StartCapture, "");
    }

    public void startPreview(SurfaceView view){
        mVideoView = view;
        if (mVideoView != null) {
            mVideoView.setVisibility(View.INVISIBLE);
            mVideoView.getHolder().addCallback(this);
            mVideoView.setVisibility(View.VISIBLE);
        }
    }

    public void release(){
        JNIContext.Instance().deleteObject(jniObject);
        jniHandler.release();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        jniHandler.putObject(0, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }
}
