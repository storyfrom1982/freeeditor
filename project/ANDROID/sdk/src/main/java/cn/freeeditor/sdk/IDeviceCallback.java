package cn.freeeditor.sdk;

public interface IDeviceCallback {

    void onProcessData(byte[] data, int length);

    void onError(String error);
}
