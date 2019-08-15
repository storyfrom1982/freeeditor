//
// Created by yongge on 19-8-14.
//

#include "StreamEditor.h"


using namespace freee;


StreamEditor::StreamEditor(const sr_msg_t &msg) : StreamCapture(msg) {

}

StreamEditor::~StreamEditor() {

}

void StreamEditor::messageFromInputStream(sr_msg_t msg) {
    StreamCapture::messageFromInputStream(msg);
}

void StreamEditor::messageFromOutputStream(sr_msg_t msg) {
    StreamCapture::messageFromOutputStream(msg);
}

sr_msg_t StreamEditor::requestFromInputStream(sr_msg_t msg) {
    return StreamCapture::requestFromInputStream(msg);
}

sr_msg_t StreamEditor::requestFromOutputStream(sr_msg_t msg) {
    return StreamCapture::requestFromOutputStream(msg);
}
