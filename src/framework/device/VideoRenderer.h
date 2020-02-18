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

    class VideoRenderer : public SrMessageQueue {

    public:
        VideoRenderer();
        ~VideoRenderer() override;

    public:
        void OnPutMessage(SrMessage msg);

    protected:
        void MessageProcessor(SrMessage msg) override;

    protected:


    private:
//        void messageProcessorLoop(sr_message_processor_t *processor, sr_message_t msg);
//        static void messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg);

        void init(SrMessage msg);
        void surfaceCreated(SrMessage msg);
        void surfaceChanged(SrMessage msg);
        void surfaceDestroyed(SrMessage msg);

        void setSurfaceView(SrMessage msg);
        void drawPicture(SrMessage msg);

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
