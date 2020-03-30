//
// Created by yongge on 19-5-13.
//

#ifndef ANDROIDFREERTC_MCONFIG_H
#define ANDROIDFREERTC_MCONFIG_H


#include <string>
#include <json.hpp>


#ifdef __cplusplus
extern "C" {
#endif

# include <sr_malloc.h>
# include <sr_library.h>
# include <resampler/libyuv_resampler.h>

#ifdef __cplusplus
}
#endif


using json = nlohmann::json;

namespace freee {


#define CFG_TYPE                        "type"

#define CFG_AUDIO                       "audio"
#define CFG_VIDEO                       "video"

#define CFG_SRC_WIDTH                   "srcWidth"
#define CFG_SRC_HEIGHT                  "srcHeight"
#define CFG_SRC_IMAGE_FORMAT            "srcImageFormat"
#define CFG_SRC_VIDEO_DEVICE            "srcVideoDevice"
#define CFG_SRC_ROTATION                "srcRotation"

#define CFG_SRC_SAMPLE_RATE             "srcSampleRate"
#define CFG_SRC_SAMPLE_FORMAT           "srcSampleFormat"
#define CFG_SRC_BYTES_PER_SAMPLE         "srcBytesPerSample"
#define CFG_SRC_SAMPLES_PER_FRAME       "srcSamplesPerFrame"
#define CFG_SRC_CHANNEL_COUNT           "srcChannelCount"
#define CFG_SRC_AUDIO_DEVICE            "srcAudioDevice"

#define CFG_CODEC_ID                    "codecId"
#define CFG_CODEC_STREAM_ID             "codecStreamId"
#define CFG_CODEC_LEVEL                 "codecLevel"
#define CFG_CODEC_PROFILE               "codecProfile"

#define CFG_CODEC_TYPE                  "codecType"
#define CFG_CODEC_NAME                  "codecName"
#define CFG_CODEC_BITRATE               "codecBitrate"

#define CFG_CODEC_WIDTH                 "codecWidth"
#define CFG_CODEC_HEIGHT                "codecHeight"
#define CFG_CODEC_PIXEL_WIDTH           "codecPixelWidth"
#define CFG_CODEC_PIXEL_HEIGHT          "codecPixelHeight"
#define CFG_CODEC_FRAME_RATE            "codecFrameRate"
#define CFG_CODEC_IMAGE_FORMAT          "codecImageFormat"
#define CFG_CODEC_IMAGE_FORMAT_ID       "codecImageFormatId"
#define CFG_CODEC_KEY_FRAME_INTERVAL    "codecKeyFrameInterval"
#define CFG_CODEC_RATE_CONTROL_MODE     "codecRateControlMode"

#define CFG_CODEC_SAMPLE_RATE           "codecSampleRate"
#define CFG_CODEC_SAMPLE_FORMAT         "codecSampleFormat"
#define CFG_CODEC_SAMPLE_FORMAT_ID      "codecSampleFormatId"
#define CFG_CODEC_CHANNEL_COUNT         "codecChannelCount"
#define CFG_CODEC_BYTES_PER_SAMPLE      "codecBytesPerSample"
#define CFG_CODEC_SAMPLES_PER_FRAME     "codecSamplesPerFrame"

#define CFG_RENDER_MODE                 "RenderMode"
#define CFG_RENDER_ROTATION             "RenderRotation"
#define CFG_RENDER_MIRROR               "RenderMirror"

#define CFG_SPEAKER_SAMPLE_RATE         "speakerSampleRate"
#define CFG_SPEAKER_SAMPLE_FORMAT       "speakerSampleFormat"
#define CFG_SPEAKER_CHANNEL_COUNT       "speakerChannelCount"


    class MediaConfig {

    public:

        static json load(std::string configPath = "");
        static void save(json &config, std::string configPath);


    private:

        static void buildDefaultConfig(json &config);
    };

}


#endif //ANDROIDFREERTC_MCONFIG_H
