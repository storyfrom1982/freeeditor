package cn.freeeditor.sdk;

public class MsgKey {

    public static final int Null = -1;
    public static final int OK = 0;
    public static final int ProcessData = 1;
    public static final int UpdateConfig = 2;

    public static final int EnvCtx_HomePath = 3;
    public static final int EnvCtx_StoragePath = 4;
    public static final int EnvCtx_LoadConfig = 5;
    public static final int EnvCtx_SaveConfig = 6;
    public static final int EnvCtx_UpdateConfig = 7;
    public static final int EnvCtx_SystemInfo = 8;
    public static final int EnvCtx_SystemStatus = 9;
    public static final int EnvCtx_CreateCapture = 10;
    public static final int EnvCtx_RemoveCapture = 11;
    public static final int EnvCtx_CreatePlayer = 12;
    public static final int EnvCtx_RemovePlayer = 13;


    public static final int Editor_Begin = 1000;
    public static final int Editor_LoadConfig = 1001;
    public static final int Editor_SaveConfig = 1002;
    public static final int Editor_UpdateConfig = 1003;

    public static final int Editor_SetTextView = 1004;
    public static final int Editor_SetVideoView = 1005;
    public static final int Editor_SetAudioView = 1006;

    public static final int Editor_OpenTextSource = 1007;
    public static final int Editor_CloseTextSource = 1008;
    public static final int Editor_ChangeTextSource = 1009;

    public static final int Editor_OpenVideoSource = 1010;
    public static final int Editor_CloseVideoSource = 1011;
    public static final int Editor_ChangeVideoSource = 1012;

    public static final int Editor_OpenAudioSource = 1013;
    public static final int Editor_CloseAudioSource = 1014;
    public static final int Editor_ChangeAudioSource = 1015;

    public static final int Editor_StartPreview = 1016;
    public static final int Editor_StopPreview = 1017;

    public static final int Editor_PauseTextStream = 1018;
    public static final int Editor_ResumeTextStream = 1019;
    public static final int Editor_EnableTextStream = 1020;
    public static final int Editor_DisableTextStream = 1021;

    public static final int Editor_PauseAudioStream = 1022;
    public static final int Editor_ResumeAudioStream = 1023;
    public static final int Editor_EnableAudioStream = 1024;
    public static final int Editor_DisableAudioStream = 1025;

    public static final int Editor_PauseVideoStream = 1026;
    public static final int Editor_ResumeVideoStream = 1027;
    public static final int Editor_EnableVideoStream = 1028;
    public static final int Editor_DisableVideoStream = 1029;

    public static final int Editor_SetPublishURL = 1030;
    static final int Editor_StartPublish = 1031;
    static final int Editor_StopPublish = 1032;

    static final int Player_Begin = 2000;

    static final int Text_Source_Begin = 3000;

    static final int Text_Effect_Begin = 4000;

    static final int Audio_Source_Begin = 5000;

    static final int Audio_Effect_Begin = 6000;

    static final int Audio_Render_Begin = 7000;


    static final int Video_Source_Begin = 8000;
    static final int Video_Source_Open = 8001;
    static final int Video_Source_Close = 8002;
    static final int Video_Source_StartCapture = 8003;
    static final int Video_Source_StopCapture = 8004;
    static final int Video_Source_FinalConfig = 8005;
    static final int Video_Source_ProvideFrame = 8006;


    static final int Video_Effect_Begin = 9000;

    static final int Video_Render_Begin = 10000;

}
