package cn.freeeditor.sdk;

public class MsgHandler {

    public interface IMsgListener {
        void onMessage(Msg msg);
        Msg onRequest(Msg msg);
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

    public Msg requestMessage(Msg msg){
        return requestMessage(msg, mInstance);
    }

    void onMessage(Msg msg){
        mListener.onMessage(msg);
    }

    Msg onRequest(Msg msg){
        return mListener.onRequest(msg);
    }

    private long mInstance;

    private native long build();

    private native void remove(long mInstance);

    private native void sendMessage(Msg msg, long mInstance);

    private native Msg requestMessage(Msg msg, long mInstance);

    private native void setListener(long listener, long mInstance);

}
