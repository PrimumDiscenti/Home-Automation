package com.example.smarthomever5;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.time.LocalTime;
import java.time.format.DateTimeFormatter;
import java.util.Timer;
import java.util.TimerTask;

public class MainActivity extends AppCompatActivity {

    // Sensor Labels Objects
    TextView roomTempLabel, roomHumdLabel;
    TextView outTempLabel, outHumdLabel, outPressLabel, outAltdLabel;
    TextView roomTitle, weatherTitle, doorTitle;
    TextView heightLabel;

    // Other objects
    EditText codeBox;
    Button codeBtn, lightBtn, timeBtn;

    Animation scaleUp, scaleDown;

    //Bluetooth Global code
    BTDevice doorDevice;
    BTDevice roomDevice;
    BTDevice weatherDevice;
    BTDevice heightDevice;
    BTDevice[] devices;


    // Global Variable
    TextView[] senseLabels;
    TextView[] isActiveLabels;

    int activeColor = -1275068417;
    int inactiveColor = Color.RED;
    int clickedColor = Color.parseColor("#4DAF50");
    boolean lightState = true;


    @SuppressLint({"MissingInflatedId", "MissingPermission"})
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Sensors
        roomTempLabel = findViewById(R.id.roomTempTxt);
        roomHumdLabel = findViewById(R.id.roomHumdTxt);
        outTempLabel = findViewById(R.id.outTempTxt);
        outHumdLabel = findViewById(R.id.outHumdTxt);
        outPressLabel = findViewById(R.id.outPressTxt);
        outAltdLabel = findViewById(R.id.outAltdTxt);
        heightLabel = findViewById(R.id.heightTxt);
        codeBox = findViewById(R.id.codeBox);
        codeBtn = findViewById(R.id.doorCodeBtn);
        lightBtn = findViewById(R.id.lightBtn);
        timeBtn = findViewById(R.id.setTimeBtn);
        senseLabels = new TextView[]{roomTempLabel, roomHumdLabel, outTempLabel, outHumdLabel, outPressLabel, outAltdLabel, heightLabel};
        roomTitle = findViewById(R.id.roomTitle);
        weatherTitle = findViewById(R.id.weatherTitle);
        doorTitle = findViewById(R.id.codeTitle);
        isActiveLabels = new TextView[]{roomTitle, weatherTitle, doorTitle, heightLabel};

        scaleUp = AnimationUtils.loadAnimation(this, R.anim.scale_up);
        scaleDown = AnimationUtils.loadAnimation(this, R.anim.scale_down);


        // Bluetooth Related stuff
        connectThread();
        startSenseTimer();

        //App Event Listeners and object Setup
        appEventListeners();
    }

    @SuppressLint("MissingPermission")
    void connectThread(){
        BluetoothAdapter btAdapter = BluetoothAdapter.getDefaultAdapter();
        System.out.println("This is a print statement-------------");
        System.out.println(btAdapter.getBondedDevices());
        Runnable runnable1 = () -> {
            roomDevice = new BTDevice(btAdapter, "98:D3:B1:FD:FD:15", "Room");
            if(!roomDevice.isConnected())
                roomTitle.setTextColor(inactiveColor);
        };

        Runnable runnable2 = () -> {
            weatherDevice = new BTDevice(btAdapter, "98:D3:91:FE:95:78", "Weather");
            if(!weatherDevice.isConnected())
                weatherTitle.setTextColor(inactiveColor);
        };

        Runnable runnable3 = () -> {
            doorDevice = new BTDevice(btAdapter, "98:D3:51:FE:ED:28", "Door");
            if(!doorDevice.isConnected())
                doorTitle.setTextColor(inactiveColor);
        };

        Runnable runnable4 = () -> {
            heightDevice = new BTDevice(btAdapter, "98:D3:32:F5:AB:2D", "Height");
            if(!heightDevice.isConnected())
                heightLabel.setTextColor(inactiveColor);
        };

        Thread thread1 = new Thread(runnable1);
        Thread thread2 = new Thread(runnable2);
        Thread thread3 = new Thread(runnable3);
        Thread thread4 = new Thread(runnable4);

        thread1.start();
        thread2.start();
        thread3.start();
        thread4.start();
    }

    final Handler handler = new Handler();
    Timer timer = new Timer();
    private void startSenseTimer(){
        TimerTask task = new TimerTask() {
            public void run() {
                handler.post(new Runnable() {
                    public void run() {
                        //Reading in values
                        if(weatherDevice != null)
                        if(weatherDevice.isConnected() && weatherDevice != null){
                            weatherDevice.read(500,0,100);
                            String[] weatherArr = weatherDevice.inputText.trim().split("\t");
                            if(weatherArr.length == 4){
                                outTempLabel.setText(String.format("Temp: %s °C", weatherArr[0]));
                                outPressLabel.setText(String.format("Press: %s hpa", weatherArr[1]));
                                outHumdLabel.setText(String.format("Humd: %s %%", weatherArr[2]));
                                outAltdLabel.setText(String.format("Altd: %s m", weatherArr[3]));
                            }
                        }
                        if(roomDevice != null)
                        if(roomDevice.isConnected()){
                            roomDevice.read(500, 5,12);
                            String[] roomArr = roomDevice.inputText.trim().split("\t");
                            if(roomArr.length == 2){
                                roomTempLabel.setText(String.format("Temp: %s °C", roomArr[0]));
                                roomHumdLabel.setText(String.format("Humd: %s %%", roomArr[1]));
                            }
                        }
                        if(heightDevice != null)
                        if(heightDevice.isConnected()){
                            heightDevice.read(500, 5, 10);
                            String[] heightArr = heightDevice.inputText.trim().split("\t");
                            if(heightArr.length == 2){
                                if(heightArr[0].trim().equals("1"))
                                    heightLabel.setText(String.format("Your height: %s f'i\"", heightArr[1]));
                                else
                                    heightLabel.setText(String.format("Your height: %s cm", heightArr[1]));
                            }
                        }
                    }
                });
            }
        };
        timer.scheduleAtFixedRate(task,1000,1000);
    }

    // Bluetooth Button Outputs--------------------------------------------------------------------/
    String timeStr = "";
    public void timeBtnClicked(){
        DateTimeFormatter dtf = DateTimeFormatter.ofPattern("HH:mm:ss");
        LocalTime localTime = LocalTime.now();
        System.out.println("clicked");
        timeStr = dtf.format(localTime);
        System.out.println(timeStr);
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                synchronized (this){
                    try {
                        wait(300);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
                if(roomDevice != null)
                if(roomDevice.isConnected()) {
                    roomDevice.write(roomDevice.btSocket, "|" +timeStr+"|");
                    System.out.println("PASSED---" + roomDevice.btSocket.isConnected());
                }
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }

    public void lightBtnClicked(){
        System.out.println("Light State: " + lightState);
        String lightStr = (lightState)? "1": "0";
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                synchronized (this){
                    try {
                        wait(300);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
                if(roomDevice != null)
                if(roomDevice.isConnected()) {
                    roomDevice.write(roomDevice.btSocket, "|" + lightStr +"|");
                    System.out.println("PASSED---" + roomDevice.btSocket.isConnected());
                }
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }

    public void doorBtnClicked(){
        String passCode = codeBox.getText().toString().trim();
        System.out.println(passCode);
        if(passCode.length() != 4) return;
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                synchronized (this){
                    try {
                        wait(300);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
                if(doorDevice != null)
                if(doorDevice.isConnected()) {
                    doorDevice.write(doorDevice.btSocket, "|" +passCode+"|");
                    System.out.println("PASSED---" + doorDevice.btSocket.isConnected());
                }
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }



    // Non Bluetooth-dependent App listener code---------------------------------------------------/
    @SuppressLint({"ClickableViewAccessibility", "UseCompatLoadingForDrawables", "SetTextI18n"})
    void appEventListeners(){
        // Light Button Events
        lightBtn.setOnTouchListener((view, motionEvent) -> {
            if(motionEvent.getAction() == MotionEvent.ACTION_DOWN){
                lightBtn.startAnimation(scaleUp);
                if(lightState){
                    lightBtn.setBackground(getDrawable(R.drawable.btnon));
                    lightBtn.setTextColor(Color.parseColor("#CCB089"));
                    lightBtn.setText("ON");
                }
                else{
                    lightBtn.setBackground(getDrawable(R.drawable.btn_off));
                    lightBtn.setTextColor(Color.parseColor("#025F4D"));
                    lightBtn.setText("OFF");
                }
                lightBtnClicked();
                lightState = !lightState;

            }
            else if(motionEvent.getAction() == MotionEvent.ACTION_UP)
                lightBtn.startAnimation(scaleDown);

            return true;
        });
        // TimeButton Events
        timeBtn.setOnTouchListener((view, motionEvent) -> {
            if(motionEvent.getAction() == MotionEvent.ACTION_DOWN){
                timeBtn.startAnimation(scaleUp);
                timeBtn.setBackground(getResources().getDrawable(R.drawable.btnon));
            }
            else if(motionEvent.getAction() == MotionEvent.ACTION_UP){
                timeBtn.startAnimation(scaleDown);
                timeBtn.setBackground(getResources().getDrawable(R.drawable.btn_off));
            }
            timeBtnClicked();

            return true;
        });
        // Code button Events
        codeBtn.setOnTouchListener((view, motionEvent) -> {
            if(motionEvent.getAction() == MotionEvent.ACTION_DOWN)
                codeBtn.startAnimation(scaleUp);
            else if(motionEvent.getAction() == MotionEvent.ACTION_UP)
                codeBtn.startAnimation(scaleDown);
            doorBtnClicked();
            return true;
        });


    }
    //Properly Closing Bt Sockets
    @Override
    protected void onDestroy() {
        super.onDestroy();
        weatherDevice.close();
        roomDevice.close();
        doorDevice.close();
        heightDevice.close();

    }

    public void reConnectRoom(View view) {
        Runnable runnable = () -> {
            isActiveLabels[0].setTextColor(clickedColor);
            roomDevice = new BTDevice(BluetoothAdapter.getDefaultAdapter(), "98:D3:B1:FD:FD:15", "Room");
            if(!roomDevice.isConnected()){
                isActiveLabels[0].setTextColor(inactiveColor);
            }else isActiveLabels[0].setTextColor(activeColor);
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }
    public void reConnectWeather(View view) {
        Runnable runnable = () -> {
            isActiveLabels[1].setTextColor(clickedColor);
            weatherDevice = new BTDevice(BluetoothAdapter.getDefaultAdapter(), "98:D3:91:FE:95:78", "Weather");
            if(!weatherDevice.isConnected()){
                isActiveLabels[1].setTextColor(inactiveColor);
            }else isActiveLabels[1].setTextColor(activeColor);
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }
    public void reConnectDoor(View view) {
        Runnable runnable = () -> {
            isActiveLabels[2].setTextColor(clickedColor);
            doorDevice = new BTDevice(BluetoothAdapter.getDefaultAdapter(), "98:D3:51:FE:ED:28", "Door");
            if(!doorDevice.isConnected()){
                isActiveLabels[2].setTextColor(inactiveColor);
            }else isActiveLabels[2].setTextColor(activeColor);
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }
    public void reConnectHeight(View view) {
        Runnable runnable = () -> {
            isActiveLabels[3].setTextColor(clickedColor);
            heightDevice = new BTDevice(BluetoothAdapter.getDefaultAdapter(), "98:D3:32:F5:AB:2D", "Height");
            if(!heightDevice.isConnected()){
                isActiveLabels[3].setTextColor(inactiveColor);
            }else isActiveLabels[3].setTextColor(activeColor);
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }
}