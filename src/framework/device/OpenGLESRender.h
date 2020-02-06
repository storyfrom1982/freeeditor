//
// Created by yongge on 20-2-4.
//

#ifndef ANDROID_OPENGLESRENDERER_H
#define ANDROID_OPENGLESRENDERER_H


#include <DeviceContext.h>

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
        OpenGLESRender_Init = 0,
        OpenGLESRender_SetSurfaceView,
        OpenGLESRender_DrawPicture,
    };

    class OpenGLESRender : public DeviceContext {
    public:
        OpenGLESRender(DeviceContext *ctx);

        ~OpenGLESRender() override;

    public:
        int onPutObject(int type, void *obj) override;

        void *onGetObject(int type) override;

        int onPutMessage(int cmd, std::string msg) override;

        std::string onGetMessage(int cmd) override;

        int onPutData(void *data, int size) override;

        void *onGetBuffer() override;


    private:
        void messageProcessorLoop(sr_msg_processor_t *processor, sr_msg_t msg);
        static void messageProcessorThread(sr_msg_processor_t *processor, sr_msg_t msg);

        void init(sr_msg_t msg);
        void setSurfaceView(sr_msg_t msg);
        void drawPicture(sr_msg_t msg);

    private:

        sr_msg_queue_t *m_queue;
        sr_msg_processor_t m_processor;

        gl_window_t *window;
        gl_renderer_t *renderer;
        opengles_t *opengles;

    };

}


#endif //ANDROID_OPENGLESRENDERER_H
