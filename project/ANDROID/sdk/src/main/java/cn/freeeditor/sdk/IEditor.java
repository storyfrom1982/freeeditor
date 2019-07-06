package cn.freeeditor.sdk;

import android.view.SurfaceView;

public interface IEditor {

    void setListener(MsgHandler.IMsgListener listener);

    void setResolution(int width, int height);

    void startPreview();

    void stopPreview();

    void startPushStream();

    void stopPushStream();

    void setPreviewSurface(SurfaceView view);
}
