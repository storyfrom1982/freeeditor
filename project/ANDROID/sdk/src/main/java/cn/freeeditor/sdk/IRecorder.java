package cn.freeeditor.sdk;

import android.view.SurfaceView;

public interface IRecorder {

    void setListener(MsgHandler.IMsgListener listener);

    void setResolution(int width, int height);

    void startPreview();

    void stopPreview();

    void startPushStream();

    void stopPushStream();

    void setPreviewSurface(SurfaceView view);
}
