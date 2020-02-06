//
// Created by yongge on 19-6-16.
//

#ifndef ANDROID_ANDROIDCAMERA_H
#define ANDROID_ANDROIDCAMERA_H


#include <VideoSource.h>


namespace freee {

    class Camera : public VideoSource {

    public:
        Camera(DeviceContext *ctx);
        ~Camera();

        virtual void openSource(json cfg) override ;
        virtual void closeSource() override ;

        virtual void startCapture() override ;
        virtual void stopCapture() override ;

    protected:
        int onPutObject(int type, void *obj) override;

        void *onGetObject(int type) override;

        int onPutMessage(int cmd, std::string msg) override;

        std::string onGetMessage(int cmd) override;

        int onPutData(void *data, int size) override;

        void *onGetBuffer() override;

        int
        imageFilter(void *src, int src_w, int src_h, int src_fmt, void *dst, int dst_w, int dst_h,
                    int dst_fmt, int rotate) override;

    };
}



#endif //ANDROID_ANDROIDCAMERA_H
