package cn.freeeditor.sdk.permission;

import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.ResultReceiver;

/**
 * Created by Farruxx on 30.04.2016.
 */
public class PermissionRequest {
    Context context;
    String[] permissions;
    int requestCode;

    PermissionResponse response;


    public PermissionRequest(Context context, String[] permissions, int requestCode){
        this.context = context;
        this.permissions = permissions;
        this.requestCode= requestCode;
    }

    /**
     * 不可以在UI线程中直接调用，否则会阻塞UI线程
     * @return
     * @throws InterruptedException
     */
    public PermissionResponse call() throws InterruptedException {
        if(!Util.hasPermission(context, permissions)) {

            final Object lock = new Object();

            Intent intent = new Intent(context, PermissionActivity.class);
            intent.putExtra(Const.REQUEST_CODE, requestCode);
            intent.putExtra(Const.PERMISSIONS_ARRAY, permissions);
            intent.putExtra(Const.RESULT_RECEIVER, new ResultReceiver(new Handler(Looper.getMainLooper())) {
                @Override
                protected void onReceiveResult(int resultCode, Bundle resultData) {
                    super.onReceiveResult(resultCode, resultData);
                    int[] grantResult = resultData.getIntArray(Const.GRANT_RESULT);
                    String[] permissions = resultData.getStringArray(Const.PERMISSIONS_ARRAY);
                    response = new PermissionResponse(permissions, grantResult, resultCode);
                    synchronized (lock) {
                        lock.notifyAll();
                    }
                }
            });
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_DOCUMENT);
            context.startActivity(intent);

            synchronized (lock) {
                lock.wait();
            }

        }else {
            response = new PermissionResponse(permissions,
                    new int[]{PackageManager.PERMISSION_GRANTED}, requestCode);
        }
        return response;
    }

    public void enqueue(final PermissionResultCallback callback){
        if(!Util.hasPermission(context, permissions)) {
            Intent intent = new Intent(context, PermissionActivity.class);
            intent.putExtra(Const.REQUEST_CODE, requestCode);
            intent.putExtra(Const.PERMISSIONS_ARRAY, permissions);
            intent.putExtra(Const.RESULT_RECEIVER, new ResultReceiver(new Handler(Looper.getMainLooper())) {
                @Override
                protected void onReceiveResult(int resultCode, Bundle resultData) {
                    super.onReceiveResult(resultCode, resultData);
                    int[] grantResult = resultData.getIntArray(Const.GRANT_RESULT);
                    String[] permissions = resultData.getStringArray(Const.PERMISSIONS_ARRAY);
                    response = new PermissionResponse(permissions, grantResult, resultCode);
                    callback.onComplete(new PermissionResponse(permissions, grantResult, resultCode));
                }
            });
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
        }else {
            callback.onComplete(new PermissionResponse(permissions,
                    new int[]{PackageManager.PERMISSION_GRANTED}, requestCode));
        }
    }
}
