//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_SR_MEDIA_H
#define ANDROID_SR_MEDIA_H


typedef struct vfmt__
{
    int width;
    int height;
    int fps;
    int code_name;
}vfmt__t;


typedef struct afmt__
{
    int sample_rate;
    int sample_size;
    int channels;
    int code_name;
}afmt__t;


//typedef struct vcode__
//{
//    vfmt__t fmt;
//    int bit_rate;
//    int iframe_interval;
//}vcode__t;
//
//
//typedef struct acode__
//{
//    afmt__t fmt;
//    int bit_rate;
//    int samples_per_frame;
//}acode__t;


#endif //ANDROID_SR_MEDIA_H
