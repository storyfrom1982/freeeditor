//
// Created by yongge on 20-2-22.
//

#ifndef ANDROID_VIDEORENDERER_H
#define ANDROID_VIDEORENDERER_H


#include <MediaModule.h>
#include <MessageProcessor.h>
#include "VideoWindow.h"


#ifdef __cplusplus
extern "C" {
#endif

# include <GLRenderer.h>
# include <opengles.h>

#ifdef __cplusplus
}
#endif


namespace freee {


    class VideoRenderer : public MediaModule, VideoWindow::VideoSurfaceCallback {

    public:
        VideoRenderer(
                int mediaType = MediaType_Video,
                int mediaNumber = MediaNumber_VideoRender,
                const std::string &mediaName = "VideoRenderer");
        ~VideoRenderer();


    public:
        void SetVideoWindow(MediaPacket pkt);

    private:
        void MessageSetVideoWindow(MediaPacket pkt);

        void MessageWindowCreated(MediaPacket pkt);

        void MessageWindowDestroyed(MediaPacket pkt);

        void MessageWindowChanged(MediaPacket pkt);

        void onSurfaceCreated(MediaPacket msg) override;

        void onSurfaceChanged(MediaPacket msg) override;

        void onSurfaceDestroyed(MediaPacket msg) override;

        void UpdateViewport(int width, int height);


    private:
        void MessageOpen(MediaPacket pkt) override;

        void MessageClose(MediaPacket pkt) override;

        void MessageStart(MediaPacket pkt) override;

        void MessageStop(MediaPacket pkt) override;

        void MessagePacket(MediaPacket pkt) override;

        void MessageControl(MediaPacket pkt) override;


    private:
        int ModuleImplOpen(json &cfg) override;

        void ModuleImplClose() override;

        int ModuleImplProcessMedia(MediaPacket pkt) override;


    private:
        int mStatus;

        gl_window_t *window;
        gl_renderer_t *renderer;
        opengles_t *opengles;

        Lock mLock;
        bool isWindowReady;
        bool isWindowPausing;
        VideoWindow *mVideoWindow;
    };

}


#endif //ANDROID_VIDEORENDERER_H
