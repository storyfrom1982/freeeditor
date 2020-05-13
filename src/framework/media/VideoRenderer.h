//
// Created by yongge on 20-2-22.
//

#ifndef ANDROID_VIDEORENDERER_H
#define ANDROID_VIDEORENDERER_H


#include "../message/MessageProcessor.h"
#include "VideoWindow.h"
#include "../message/MessageChain.h"


#ifdef __cplusplus
extern "C" {
#endif

# include <GLRenderer.h>
# include <opengles.h>

#ifdef __cplusplus
}
#endif


namespace freee {


    class VideoRenderer : public MessageChain, VideoWindow::VideoWindowCallback {

    public:
        VideoRenderer(const std::string name = "VideoRenderer");
        virtual ~VideoRenderer();


    public:
        void SetVideoWindow(void *ptr);

    private:
        void FinalClear() override ;

        void MessageSetVideoWindow(Message pkt);

        void MessageWindowCreated(Message pkt);

        void MessageWindowDestroyed(Message pkt);

        void MessageWindowChanged(Message pkt);

        void onSurfaceCreated(void *ptr) override;

        void onSurfaceChanged(int width, int height) override;

        void onSurfaceDestroyed() override;

        void UpdateViewport(int width, int height);


    private:
        void onMsgOpen(Message pkt) override;

        void onMsgClose(Message pkt) override;

        void onMsgProcessData(Message pkt) override;

        void onMsgControl(Message pkt) override;


    private:
        int OpenMedia() override;

        void CloseMedia() override;

        int ProcessMedia(Message pkt) override;


    private:
        int width, height;

        gl_window_t *window;
        gl_renderer_t *renderer;
        opengles_t *opengles;

        Lock mLock;
        bool isSurfaceCreated;
        bool isSurfaceDestroyed;
        VideoWindow *mVideoWindow;
    };

}


#endif //ANDROID_VIDEORENDERER_H
