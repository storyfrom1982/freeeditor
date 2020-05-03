/*
 * opengles.h
 *
 *  Created on: 2014-3-12
 *      Author: kly
 */

#ifndef OPENGLES_H_
#define OPENGLES_H_

#include <resampler/libyuv_resampler.h>


typedef struct opengles_t opengles_t;
int opengles_open(opengles_t **pp_gles);
void opengles_close(opengles_t **pp_gles);

int opengles_render(opengles_t *gles, const sr_message_frame_t *frame);

#endif /* OPENGLES_H_ */
