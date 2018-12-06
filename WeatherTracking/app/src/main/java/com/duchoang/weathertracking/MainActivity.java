package com.duchoang.weathertracking;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import java.net.ServerSocket;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;



public class MainActivity extends Activity {
    private final static String TAG = "IOTProject@DH";
    private ServerSocket serverSocket;
    Handler updateConversationHandler;
    Thread serverThread = null;
    public static final int SERVERPORT = 8080;
    private RequestQueue queue;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        updateConversationHandler = new Handler();
        this.serverThread = new Thread(new ServerThread());
        this.serverThread.start();
    }

    @Override
    protected void onStop() {
        super.onStop();
        try {
            serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private class ServerThread implements Runnable {
        public void run() {
            Socket socket = null;
            try {
                serverSocket = new ServerSocket(SERVERPORT);
            } catch (IOException e) {
                Log.e(TAG, "Error on creating Server Thread!");
            }
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    socket = serverSocket.accept();
                    CommunicationThread commThread = new CommunicationThread(socket);
                    new Thread(commThread).start();
                } catch (IOException e) {
                    Log.e(TAG, "Error on creating Communication Thread!");
                }
            }
        }
    }

    private class CommunicationThread implements  Runnable {
        private Socket clientSocket;
        private BufferedReader input;
        private CommunicationThread(Socket clientSocket) {
            this.clientSocket = clientSocket;
            try {
                this.input = new BufferedReader(new InputStreamReader(this.clientSocket.getInputStream()));
            } catch (IOException e) {
                Log.e(TAG, "Error");
            }
        }
        public void run() {
            while (!Thread.currentThread().isInterrupted()) {
                try {
                    String read = input.readLine();
                    updateConversationHandler.post(new updateString(read));
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        private class updateString implements Runnable {
            private String msg;
            private updateString(String str) {
                this.msg = str;
            }
            @Override
            public void run() {
                Log.i(TAG, "Data received: " + msg);
                sendRequest(msg);
            }
        }
    }


    // As a HTTP Client
    private void sendRequest(String str) {
        if (queue == null) {
            queue = Volley.newRequestQueue(this);
        } else {
            String urlTemp = "https://immense-reaches-55030.herokuapp.com/temp/";
            String urlHumid = "https://immense-reaches-55030.herokuapp.com/humid/";
            String urlLight = "https://immense-reaches-55030.herokuapp.com/light/";
            if (str.contains("temp")) {
                String stringToSend = str.substring(6, 8);
                StringRequest stringRequest = new StringRequest(Request.Method.GET, urlTemp+stringToSend, new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        Log.i(TAG, "Temp response is: " + response);
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Log.e(TAG, "Things do not work!");
                    }
                });
                queue.add(stringRequest);
            }
            else if (str.contains("light")) {
                String stringToSend = str.substring(7);
                StringRequest stringRequest = new StringRequest(Request.Method.GET, urlLight+stringToSend, new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        Log.i(TAG, "Light response is: " + response);
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Log.e(TAG, "Things do not work!");
                    }
                });
                queue.add(stringRequest);
            }
            else if (str.contains("humid")) {
                String stringToSend = str.substring(7, 9);
                StringRequest stringRequest = new StringRequest(Request.Method.GET, urlHumid+stringToSend, new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        Log.i(TAG, "Humid response is: " + response);
                    }
                }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Log.e(TAG, "Things do not work!");
                    }
                });
                queue.add(stringRequest);
            }
            else {
                Log.i(TAG, "Unrecognized data: " + str);
            }
        }
    }

}
