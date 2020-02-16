//
// Created by yongge on 20-2-15.
//

#include "AudioEncoder.h"
#include "FdkaacEncoder.h"


using namespace freee;

AudioEncoder *AudioEncoder::Create(std::string name) {
    return new FdkaacEncoder();
}
