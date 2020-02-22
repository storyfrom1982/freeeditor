//
// Created by yongge on 20-2-4.
//

#ifndef ANDROID_OPENGLESRENDERER_H
#define ANDROID_OPENGLESRENDERER_H


#include <MessageContext.h>
#include <AutoLock.h>
#include <SrMessageQueue.h>

#ifdef __cplusplus
extern "C" {
#endif

# include <GLRenderer.h>
# include <opengles.h>

#ifdef __cplusplus
}
#endif

namespace freee {

    enum {
        OpenGLESRender_Init = 1,
        OpenGLESRender_SetSurfaceView,
        OpenGLESRender_DrawPicture,
        OpenGLESRender_SurfaceCreated,
        OpenGLESRender_SurfaceDestroyed,
    };

    class MyVideoRenderer : public SrMessageQueue {

    public:
        MyVideoRenderer();
        ~MyVideoRenderer() override;

    public:
        void OnPutMessage(MediaPacket pkt);

    protected:
        void MessageProcessor(MediaPacket pkt) override;

    protected:


    private:
//        void messageProcessorLoop(sr_message_processor_t *processor, sr_message_t msg);
//        static void messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg);

        void init(MediaPacket pkt);
        void surfaceCreated(MediaPacket pkt);
        void surfaceChanged(MediaPacket msg);
        void surfaceDestroyed(MediaPacket msg);

        void setSurfaceView(MediaPacket pkt);
        void drawPicture(MediaPacket pkt);

    private:

//        sr_message_queue_t *m_queue;
//        sr_message_processor_t m_processor;

        gl_window_t *window;
        gl_renderer_t *renderer;
        opengles_t *opengles;

        Lock m_lock;

    };

}


#endif //ANDROID_OPENGLESRENDERER_H
