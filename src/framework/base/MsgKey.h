//
// Created by yongge on 19-7-6.
//

#ifndef ANDROID_MSGKEY_H
#define ANDROID_MSGKEY_H

enum {

    MsgKey_Context_HomePath = 3,
    MsgKey_Context_ConfigPath,
    MsgKey_Context_LoadConfig,
    MsgKey_Context_SaveConfig,
    MsgKey_Context_UpdateConfig,
    MsgKey_Context_SystemInfo,
    MsgKey_Context_SystemStatus,
    MsgKey_Context_NewEditor = 900,
    MsgKey_Context_RemoveEditor,
    MsgKey_Context_NewPlayer,
    MsgKey_Context_RemovePlayer,


    MsgKey_Editor_Begin = 1000,
    MsgKey_Editor_LoadConfig,
    MsgKey_Editor_SaveConfig,
    MsgKey_Editor_UpdateConfig,

    MsgKey_Editor_SetTextView,
    MsgKey_Editor_SetVideoView,
    MsgKey_Editor_SetAudioView,

    MsgKey_Editor_OpenTextSource,
    MsgKey_Editor_CloseTextSource,
    MsgKey_Editor_ChangeTextSource,

    MsgKey_Editor_OpenVideoSource,
    MsgKey_Editor_CloseVideoSource,
    MsgKey_Editor_ChangeVideoSource,

    MsgKey_Editor_OpenAudioSource,
    MsgKey_Editor_CloseAudioSource,
    MsgKey_Editor_ChangeAudioSource,

    MsgKey_Editor_StartPreview,
    MsgKey_Editor_StopPreview,

    MsgKey_Editor_PauseTextStream,
    MsgKey_Editor_ResumeTextStream,
    MsgKey_Editor_EnableTextStream,
    MsgKey_Editor_DisableTextStream,

    MsgKey_Editor_PauseAudioStream,
    MsgKey_Editor_ResumeAudioStream,
    MsgKey_Editor_EnableAudioStream,
    MsgKey_Editor_DisableAudioStream,

    MsgKey_Editor_PauseVideoStream,
    MsgKey_Editor_ResumeVideoStream,
    MsgKey_Editor_EnableVideoStream,
    MsgKey_Editor_DisableVideoStream,

    MsgKey_Editor_SetPublishURL,
    MsgKey_Editor_StartPublish,
    MsgKey_Editor_StopPublish,


    MsgKey_Player_Begin = 2000,
    MsgKey_Text_Source_Begin = 3000,
    MsgKey_Text_Effect_Begin = 4000,
    MsgKey_Audio_Source_Begin = 5000,
    MsgKey_Audio_Effect_Begin = 6000,
    MsgKey_Audio_Render_Begin = 7000,


    MsgKey_Video_Source_Begin = 8000,
    MsgKey_Video_Source_Open,
    MsgKey_Video_Source_Close,
    MsgKey_Video_Source_StartCapture,
    MsgKey_Video_Source_StopCapture,
    MsgKey_Video_Source_FinalConfig,
    MsgKey_Video_Source_ProvideFrame,

    MsgKey_Video_Effect_Begin = 9000,
    MsgKey_Video_Render_Begin = 10000,
};


#endif //ANDROID_MSGKEY_H
