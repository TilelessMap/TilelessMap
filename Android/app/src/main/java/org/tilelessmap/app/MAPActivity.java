package org.tilelessmap.app;

import org.libsdl.app.SDLActivity;

//import android.location.Location;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Service;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.*;
import android.provider.Settings;
import android.util.Log;
import android.support.v4.content.ContextCompat;
import android.support.v4.app.ActivityCompat;
import android.content.pm.PackageManager;

import java.io.File;

/*
 * A sample wrapper class that just calls SDLActivity
 */

public class MAPActivity extends SDLActivity
{

    private static final String TAG = "TilelessMap";
    gps gps;
    private static final int MY_PERMISSION_ACCESS_FINE_LOCATION = 20;
    private static final int MY_PERMISSION_ACCESS_WRITE_EXTERNAL_STORAGE = 21;
    public static native void onNativehaveDB(String the_file, String the_dir);
    String the_file;
    String the_dir;



void get_file() {

    FileChooser FC = new FileChooser(this);
    FC.setFileListener(new FileChooser.FileSelectedListener() {
        @Override
        public void fileSelected(final File file) {
            the_file = file.getAbsolutePath();
            the_dir = file.getParent();
            onNativehaveDB(the_file, the_dir);
            // Set up the surface
        }
    });

    FC.setExtension("tileless");
    FC.showDialog();

}


    protected void onCreate(Bundle savedInstanceState)
    {

        super.onCreate(savedInstanceState);

        if ( ContextCompat.checkSelfPermission( this, android.Manifest.permission.WRITE_EXTERNAL_STORAGE ) != PackageManager.PERMISSION_GRANTED )
        {
            ActivityCompat.requestPermissions( this, new String[] {android.Manifest.permission.WRITE_EXTERNAL_STORAGE  },MY_PERMISSION_ACCESS_WRITE_EXTERNAL_STORAGE );
        }
        else
        {
            get_file();
        }
        if ( ContextCompat.checkSelfPermission( this, android.Manifest.permission.ACCESS_FINE_LOCATION ) != PackageManager.PERMISSION_GRANTED )
        {
            ActivityCompat.requestPermissions( this, new String[] {android.Manifest.permission.ACCESS_FINE_LOCATION  },MY_PERMISSION_ACCESS_FINE_LOCATION );
        }
        else
        {
            gps = new gps(this);
        }





    }


    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults){
        switch (requestCode) {
            case MY_PERMISSION_ACCESS_WRITE_EXTERNAL_STORAGE: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                get_file();
                }
            }
            case MY_PERMISSION_ACCESS_FINE_LOCATION: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    gps = new gps(this);

                }
            }
        }
    }



    protected void onDestroy() {

    gps.stopUsingGPS();
    super.onDestroy();
    }


}




class gps extends Service implements LocationListener {

    public static native void onNativeGPS(double latitude, double longitude, double acc);
    private final Context mContext;

    // flag for GPS status
    boolean isGPSEnabled = false;

    // flag for network status
    boolean isNetworkEnabled = false;

    // flag for GPS status
    boolean canGetLocation = false;

    Location location; // location
    double latitude; // latitude
    double longitude; // longitude
    double acc; // accuracy

    // The minimum distance to change Updates in meters
    private static final long MIN_DISTANCE_CHANGE_FOR_UPDATES = 5; // 10 meters

    // The minimum time between updates in milliseconds
    private static final long MIN_TIME_BW_UPDATES = 1000 * 1 * 10; // 1 minute

    // Declaring a Location Manager
    protected LocationManager locationManager;

    public gps(Context context) {
        System.loadLibrary("main");
        this.mContext = context;
        startGPS();
    }

    public Location startGPS() {
        if ( Build.VERSION.SDK_INT >= 23)
        {

            int permission_test = ActivityCompat.checkSelfPermission( MAPActivity.getContext(), android.Manifest.permission.ACCESS_FINE_LOCATION );
            if(permission_test!= PackageManager.PERMISSION_GRANTED) {
                return null;
            }
        }

        try {
            locationManager = (LocationManager) mContext
                    .getSystemService(LOCATION_SERVICE);

            // getting GPS status
            isGPSEnabled = locationManager
                    .isProviderEnabled(LocationManager.GPS_PROVIDER);

            // getting network status
            isNetworkEnabled = locationManager
                    .isProviderEnabled(LocationManager.NETWORK_PROVIDER);

            if (!isGPSEnabled && !isNetworkEnabled) {
                // no network provider is enabled
            } else {
                this.canGetLocation = true;
                // First get location from Network Provider

                // if GPS Enabled get lat/long using GPS Services
                Log.d("GPS", "check Enabled");
                if (isGPSEnabled) {
                    Log.d("GPS", "OK,  Enabled");
                    locationManager.requestLocationUpdates(
                            LocationManager.GPS_PROVIDER,
                            MIN_TIME_BW_UPDATES,
                            MIN_DISTANCE_CHANGE_FOR_UPDATES, this);
                    Log.d("GPS Enabled", "GPS Enabled");


                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }

        return location;
    }

    /**
     * Stop using GPS listener
     * Calling this function will stop using GPS in your app
     * */
    public void stopUsingGPS(){
        if(locationManager != null){
            locationManager.removeUpdates(gps.this);
        }
    }

    /**
     * Function to get latitude
     * */
    public double getLatitude(){
        if(location != null){
            latitude = location.getLatitude();
        }

        // return latitude
        return latitude;
    }

    /**
     * Function to get longitude
     * */
    public double getLongitude(){
        if(location != null){
            longitude = location.getLongitude();
        }

        // return longitude
        return longitude;
    }

    /**
     * Function to check GPS/wifi enabled
     * @return boolean
     * */
    public boolean canGetLocation() {
        return this.canGetLocation;
    }

    /**
     * Function to show settings alert dialog
     * On pressing Settings button will lauch Settings Options
     * */
    public void showSettingsAlert(){
        AlertDialog.Builder alertDialog = new AlertDialog.Builder(mContext);

        // Setting Dialog Title
        alertDialog.setTitle("GPS is settings");

        // Setting Dialog Message
        alertDialog.setMessage("GPS is not enabled. Do you want to go to settings menu?");

        // On pressing Settings button
        alertDialog.setPositiveButton("Settings", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog,int which) {
                Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                mContext.startActivity(intent);
            }
        });

        // on pressing cancel button
        alertDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });

        // Showing Alert Message
        alertDialog.show();
    }



    public void onLocationChanged(Location location) {
        if ( Build.VERSION.SDK_INT >= 23)
        {

            int permission_test = ActivityCompat.checkSelfPermission( MAPActivity.getContext(), android.Manifest.permission.ACCESS_FINE_LOCATION );
                 if(permission_test!= PackageManager.PERMISSION_GRANTED) {
                     return;
                 }
        }

                Log.d("GPS","a change is here");
        if (locationManager != null) {
            location = locationManager
                    .getLastKnownLocation(LocationManager.GPS_PROVIDER);
            if (location != null) {
                latitude = location.getLatitude();
                longitude = location.getLongitude();
                acc = location.getAccuracy();
                onNativeGPS(latitude, longitude, acc);
                Log.d("GPS", "lat = " + latitude + "lon = " + longitude);
            }
        }

    }

    @Override
    public void onProviderDisabled(String provider) {
    }

    @Override
    public void onProviderEnabled(String provider) {
    }

    @Override
    public void onStatusChanged(String provider, int status, Bundle extras) {
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

}
