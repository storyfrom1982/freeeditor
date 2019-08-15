package cn.freeeditor.sdk;

public class MsgHandler {

    public interface IMsgListener {
        void onReceiveMessage(Msg msg);
        Msg onReceiveRequest(Msg msg);
    }

    private final IMsgListener mListener;

    public MsgHandler(IMsgListener listener){
        mListener = listener;
        mInstance = build();
    }

    public void remove(){
        remove(mInstance);
        mInstance = 0;
    }

    public long getInstance() {
        return mInstance;
    }

    public void setListener(long listener){
        setListener(listener, mInstance);
    }

    public void sendMessage(Msg msg){
        sendMessage(msg, mInstance);
    }

    public Msg sendRequest(Msg msg){
        return sendRequest(msg, mInstance);
    }

    void onReceiveMessage(Msg msg){
        mListener.onReceiveMessage(msg);
    }

    Msg onReceiveRequest(Msg msg){
        return mListener.onReceiveRequest(msg);
    }

    private long mInstance;

    private native long build();

    private native void remove(long mInstance);

    private native void sendMessage(Msg msg, long mInstance);

    private native Msg sendRequest(Msg msg, long mInstance);

    private native void setListener(long listener, long mInstance);

}
