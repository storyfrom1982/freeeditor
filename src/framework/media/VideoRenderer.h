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
        void SetVideoWindow(void *ptr);

    private:
        void FinalClear();

        void MessageSetVideoWindow(SmartPkt pkt);

        void MessageWindowCreated(SmartPkt pkt);

        void MessageWindowDestroyed(SmartPkt pkt);

        void MessageWindowChanged(SmartPkt pkt);

        void onSurfaceCreated(void *ptr) override;

        void onSurfaceChanged() override;

        void onSurfaceDestroyed() override;

        void UpdateViewport(int width, int height);


    private:
        void MessageOpen(SmartPkt pkt) override;

        void MessageClose(SmartPkt pkt) override;

        void MessageStart(SmartPkt pkt) override;

        void MessageStop(SmartPkt pkt) override;

        void MessagePacket(SmartPkt pkt) override;

        void MessageControl(SmartPkt pkt) override;


    private:
        int ModuleImplOpen(json &cfg) override;

        void ModuleImplClose() override;

        int ModuleImplProcessMedia(SmartPkt pkt) override;


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
