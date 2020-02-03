//
// Created by yongge on 20-1-21.
//

#ifndef DEVICE_BRIDGE_H
#define DEVICE_BRIDGE_H

#include <cstddef>

typedef struct Device_Bridge Device_Bridge;

size_t Device_Bridge_Send_Msg(Device_Bridge *, const char *, int size);
int Device_Bridge_Send_Data(Device_Bridge *, const unsigned char *, int size);
void Device_Bridge_Set_Msg_Callback(Device_Bridge *, void *context, size_t (*msg_cb)(void *, const char *, int size));
void Device_Bridge_Set_Data_Callback(Device_Bridge *, void *context, int (*data_cb)(void *, const unsigned char *, int size));

#endif //DEVICE_BRIDGE_H
