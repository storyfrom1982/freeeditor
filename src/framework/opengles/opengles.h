//
// Created by yongge on 19-7-10.
//

#ifndef ANDROID_OPENGLES_H
#define ANDROID_OPENGLES_H

#ifdef __ANDROID__
# ifdef __cplusplus
extern "C" {
# endif
#  include <GLES3/gl3.h>
#  include <GLES3/gl3ext.h>
# ifdef __cplusplus
}
# endif
#endif

#endif //ANDROID_OPENGLES_H
