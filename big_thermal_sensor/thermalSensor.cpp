#include "thermalSensor.h"

// variables for row/column interpolation
byte i, j, k, row, col, incr;
float intPoint, val, a, b, c, d, ii;
byte aLow, aHigh;

void updateThermalData(float* pixels[AMG_COLS * AMG_ROWS], Adafruit_AMG88xx* amg) {
    //read all the pixels
    (*amg).readPixels(*pixels);
    Serial.println("DDDDD");
}

void interpolate(float* pixels[AMG_COLS * AMG_ROWS], float HDTemp[HD_ROWS][HD_COLS]) {
    /*
    // interpolate to get a bigger screen
    // interpolate the 8 rows (interpolate the 70 column points between the 8 sensor pixels first)
    for (row = 0; row < 8; row ++) {
      for (col = 0; col < HD_COLS; col ++) {
        // get the first array point, then the next
        // also need to bump by 8 for the subsequent rows
        aLow =  col / 10 + (row * 8);
        aHigh = (col / 10) + 1 + (row * 8);
        // get the amount to interpolate for each of the 10 columns
        // here were doing simple linear interpolation mainly to keep performace high and
        // display is 5-6-5 color palet so fancy interpolation will get lost in low color depth
        intPoint =   (( pixels[aHigh] - pixels[aLow] ) / 10.0 );
        // determine how much to bump each column (basically 0-9)
        incr = col % 10;
        // find the interpolated value
        val = (intPoint * incr ) +  (*pixels)[aLow];
        // store in the 70 x 70 array
        // since display is pointing away, reverse row to transpose row data
        HDTemp[ (7 - row) * 10][col] = val;
      }
    }

    // now that we have row data with 70 columns
    // interpolate each of the 70 columns
    // forget Arduino..no where near fast enough..Teensy at > 72 mhz is the starting point
    for (col = 0; col < HD_COLS; col ++) {
      for (row = 0; row < HD_ROWS; row ++) {
        // get the first array point, then the next
        // also need to bump by 8 for the subsequent cols
        aLow =  (row / 10 ) * 10;
        aHigh = aLow + 10;
        // get the amount to interpolate for each of the 10 columns
        // here were doing simple linear interpolation mainly to keep performace high and
        // display is 5-6-5 color palet so fancy interpolation will get lost in low color depth
        intPoint =   (( HDTemp[aHigh][col] - HDTemp[aLow][col] ) / 10.0 );
        // determine how much to bump each column (basically 0-9)
        incr = row % 10;
        // find the interpolated value
        val = (intPoint * incr ) +  HDTemp[aLow][col];
        // store in the 70 x 70 array
        HDTemp[ row ][col] = val;
      }
    }*/
}

// TODO: add color interpolation
void thermalSensorTask(void* thData) {
    thermalSensorData* data = (thermalSensorData*) thData;

    updateThermalData(data->pixels, data->amg);
    //interpolate(data->pixels, data->HDTemp);
}
