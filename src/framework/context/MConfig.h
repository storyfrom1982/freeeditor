//
// Created by yongge on 19-5-13.
//

#ifndef ANDROIDFREERTC_MCONFIG_H
#define ANDROIDFREERTC_MCONFIG_H


#include <IMsgListener.h>

#include <string>
#include <json.hpp>


using json = nlohmann::json;

namespace freee {

    class MConfig {

    public:

        static void load(json &config, std::string configPath);
        static void save(json &config, std::string configPath);


    private:

        static void buildDefaultConfig(json &config);
    };

}


#endif //ANDROIDFREERTC_MCONFIG_H
