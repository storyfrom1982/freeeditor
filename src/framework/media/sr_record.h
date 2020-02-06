//
// Created by yongge on 20-2-3.
//

#ifndef ANDROID_SR_RECORD_H
#define ANDROID_SR_RECORD_H

typedef struct sr__record__ sr__record__t;

sr__record__t* sr_record__create(void *handler, void (*process_event)(void*, int event));
void sr_record__release(sr__record__t **pp_record);
int sr_record__get_error(sr__record__t *record);






#endif //ANDROID_SR_RECORD_H
