package cn.freeeditor.android;

import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.support.v4.content.FileProvider;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import java.io.File;

import cn.freeeditor.android.utils.AppUpdateManager;
import cn.freeeditor.sdk.MediaContext;
import cn.freeeditor.sdk.Log;

public class MainActivity extends AppCompatActivity {


    private Button playerButton;
    private Button publisherButton;
    private Button updateAppButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        publisherButton = findViewById(R.id.open_publisher);
        playerButton = findViewById(R.id.open_player);

        publisherButton.setOnClickListener(openPublishListener);
        playerButton.setOnClickListener(openPlayListener);

        updateAppButton = findViewById(R.id.update_app);
        updateAppButton.setOnClickListener(updateAppListener);

//        if (record == null){
//            record = new MediaRecord();
//        }
//        record.startCapture();
    }

//    MediaRecord record;

    @Override
    protected void onDestroy() {
        super.onDestroy();
//        record.release();
//        record = null;
        MediaContext.Instance().release();
        MediaContext.Instance().debug();
    }

    private final View.OnClickListener openPublishListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, RecordActivity.class);
            startActivity(intent);
        }
    };

    private final View.OnClickListener openPlayListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, PlayerActivity.class);
            startActivity(intent);
        }
    };


    private final View.OnClickListener updateAppListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {


            AppUpdateManager appUpdateManager = new AppUpdateManager(MainActivity.this);
            appUpdateManager.downloadApk("http://10.33.11.233:8000/app-release.apk", "app update", "app install");
//            appUpdateManager.resume();

//            final Context context = getApplicationContext();
//
//            // 兼容Android 8.0
//            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
//
//                //先获取是否有安装未知来源应用的权限
//                final boolean haveInstallPermission = context.getPackageManager().canRequestPackageInstalls();
//                if (!haveInstallPermission) {//没有权限
//                    // 弹窗，并去设置页面授权
//                    final AppInstallPermissionActivity.AppInstallPermissionListener listener = new AppInstallPermissionActivity.AppInstallPermissionListener() {
//                        @Override
//                        public void permissionSuccess() {
//                            installApk();
//                        }
//
//                        @Override
//                        public void permissionFail() {
//
//                        }
//                    };
//
//                    AppInstallPermissionActivity.sListener = listener;
//                    Intent intent1 = new Intent(context, AppInstallPermissionActivity.class);
//                    intent1.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
//                    context.startActivity(intent1);
//
//
//                } else {
//                    installApk();
//                }
//            } else {
//                installApk();
//            }

        }
    };


    private void installApk(){
        Uri uri;
        Intent intentInstall = new Intent();
        intentInstall.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intentInstall.setAction(Intent.ACTION_VIEW);

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.N) { // 6.0 - 7.0
            File apkFile = new File(Environment.getExternalStorageDirectory().getAbsoluteFile() + "/Download/" + "app-debug.apk");
            Log.d("MainActivityFREEE", "apk file path: " + apkFile.getAbsolutePath());
            uri = Uri.fromFile(apkFile);
        } else { // Android 7.0 以上
            File apkFile = new File(getApplicationContext().getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS), "app-release.apk");
            Log.e("MainActivityFREEE", "apk file path: " + apkFile.getAbsolutePath());
            uri = FileProvider.getUriForFile(getApplicationContext(),
                    "cn.freeeditor.android.fileProvider", apkFile);
            intentInstall.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_WRITE_URI_PERMISSION);
        }

        intentInstall.setDataAndType(uri, "application/vnd.android.package-archive");
        getApplication().startActivity(intentInstall);
    }
}
