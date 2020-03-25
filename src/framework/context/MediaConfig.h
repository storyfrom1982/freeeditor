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

    class MediaConfig {

    public:

        static json load(std::string configPath = "");
        static void save(json &config, std::string configPath);


    private:

        static void buildDefaultConfig(json &config);
    };

}


#endif //ANDROIDFREERTC_MCONFIG_H
