//
// Created by yongge on 20-2-4.
//

#ifndef ANDROID_OPENGLESRENDERER_H
#define ANDROID_OPENGLESRENDERER_H


#include <MessageContext.h>
#include <AutoLock.h>

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

    class VideoRenderer : public MessageContext {

    public:
        VideoRenderer();
        ~VideoRenderer() override;

    public:
        void OnPutMessage(sr_message_t msg) override;

        sr_message_t OnGetMessage(sr_message_t msg) override;

    protected:
        void MessageProcessor(sr_message_t msg) override;

    private:
//        void messageProcessorLoop(sr_message_processor_t *processor, sr_message_t msg);
//        static void messageProcessorThread(sr_message_processor_t *processor, sr_message_t msg);

        void init(sr_message_t msg);
        void surfaceCreated(sr_message_t msg);
        void surfaceChanged(sr_message_t msg);
        void surfaceDestroyed(sr_message_t msg);

        void setSurfaceView(sr_message_t msg);
        void drawPicture(sr_message_t msg);

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
