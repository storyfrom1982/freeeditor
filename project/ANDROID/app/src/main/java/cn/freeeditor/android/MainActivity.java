package cn.freeeditor.android;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {


    private Button playerButton;
    private Button publisherButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        publisherButton = findViewById(R.id.open_publisher);
        playerButton = findViewById(R.id.open_player);

        publisherButton.setOnClickListener(openPublishListener);
        playerButton.setOnClickListener(openPlayListener);
    }


    private final View.OnClickListener openPublishListener = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            Intent intent = new Intent();
            intent.setClass(MainActivity.this, PublisherActivity.class);
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
}
