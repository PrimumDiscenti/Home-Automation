package com.example.smarthomever5;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class BTDevice {private String TAG = "-----------";
    static final UUID mUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    String name;
    BluetoothAdapter btAdapter;
    BluetoothDevice hc05;
    BluetoothSocket btSocket = null;

    private boolean comState = false;



    private boolean isConnected = false;

    String inputText = "Nan";

    public BTDevice(BluetoothAdapter btAdapter, String address, String name) {
        this.name = name;
        this.btAdapter = btAdapter;
        hc05 = btAdapter.getRemoteDevice(address);
        connectSocket();
        isConnected = btSocket.isConnected();
    }

    @SuppressLint("MissingPermission")
    public void connectSocket(){

        int counter = 0;
        do {
            try {
                btSocket = hc05.createRfcommSocketToServiceRecord(mUUID);
                System.out.println(btSocket);
                btSocket.connect();
                System.out.println(btSocket.isConnected());
            } catch (IOException e) {
                System.out.println(btSocket.isConnected());
            }
            counter++;
        } while (!btSocket.isConnected() && counter < 3);
        System.out.println(name + ": " + btSocket.isConnected());
        Log.d(TAG, name + ": " + isConnected);
    }

    public boolean isConnected() {
        if(btSocket != null) isConnected =  btSocket.isConnected();
        return isConnected;
    }

    private String tempInpStr= "";
    public String read(int waitTime, int minLen, int maxLen){
        if(!isConnected || comState) return name + ": false";
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                synchronized (this){
                    try {
                        wait(waitTime);
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
                tempInpStr = "";
                InputStream inputStream = null;
                try {
                    inputStream = btSocket.getInputStream();
                    inputStream.skip(inputStream.available());

                    for (int i = 0; i < 50; i++) if((byte) inputStream.read() == 10) i = 51;
                    for (int i = 0; i < 50; i++) {
                        byte b = (byte) inputStream.read();
                        if(b == 10 || b == 13) i = 51;
                        tempInpStr += (char)b;
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                tempInpStr = tempInpStr.trim();
                inputText = (tempInpStr.length() > minLen && tempInpStr.length() < maxLen)? tempInpStr: inputText;
            }
        };
        Thread thread = new Thread(runnable);
        thread.start();
        return inputText;
    }


    public void close(){
        if (btSocket != null) {
            try {
                btSocket.close();
                Log.d(TAG, "Connection closed");
            } catch (IOException e) {
                Log.d(TAG, "Error while closing the connection");
            }
        }
    }
    public void write(BluetoothSocket socket, String message){
        if(!isConnected)return;
        Runnable runnable = () -> {
            comState = true;
            if(socket != null) {
                try{
                    OutputStream out = socket.getOutputStream();
                    out.write((message + "\r\n").getBytes());
                }catch(IOException e) {
                    e.printStackTrace();
                }
            }
            comState = false;
        };
        Thread thread = new Thread(runnable);
        thread.start();
    }
}
