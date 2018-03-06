package org.tilelessmap.app;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import java.io.File;


public class HomeActivity extends Activity {

    public static final String EXTRA_THE_FILE = "org.tilelessmap.app.THE_FILE";
    public static final String EXTRA_THE_DIR = "org.tilelessmap.app.THE_DIR";
    String the_file;
    String the_dir;

    private static final int MY_PERMISSION_ACCESS_WRITE_EXTERNAL_STORAGE = 21;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.home);
    }

    public void startMap(View view)
    {
        if(the_file == null || the_file.isEmpty() || the_dir == null || the_dir.isEmpty())
        {
            String mess = getResources().getString(R.string.missing_file);
            AlertDialog alertDialog = new AlertDialog.Builder(this).create();
            alertDialog.setTitle("Alert");
            alertDialog.setMessage(mess);
            alertDialog.setButton(AlertDialog.BUTTON_NEUTRAL, "OK",
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.dismiss();
                        }
                    });
            alertDialog.show();

        }
        else
        {

            Intent intent = new Intent(this, MAPActivity.class);

            intent.putExtra(EXTRA_THE_FILE, the_file);
            intent.putExtra(EXTRA_THE_DIR, the_dir);

            startActivity(intent);
        }

    }


    public void getFile(View view)
    {

        setContentView(R.layout.home);
        if ( ContextCompat.checkSelfPermission( this, android.Manifest.permission.WRITE_EXTERNAL_STORAGE ) != PackageManager.PERMISSION_GRANTED )
        {
            ActivityCompat.requestPermissions(this, new String[]{android.Manifest.permission.WRITE_EXTERNAL_STORAGE}, MY_PERMISSION_ACCESS_WRITE_EXTERNAL_STORAGE);
        }
        else
        {
            fileDialog();
        }


    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults){
        switch (requestCode) {
            case MY_PERMISSION_ACCESS_WRITE_EXTERNAL_STORAGE: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    fileDialog();
                }
            }
        }
    }

    public void fileDialog() {
        FileChooser FC = new FileChooser(this);
        FC.setFileListener(new FileChooser.FileSelectedListener() {
            @Override
            public void fileSelected(final File file) {
                the_file = file.getAbsolutePath();
                the_dir = file.getParent();
                // Set up the surface
                TextView textView = (TextView) findViewById(R.id.choosen_file);
                textView.setText(the_file);
            }
        });

        FC.setExtension("tileless");
        FC.showDialog();
    }
}
