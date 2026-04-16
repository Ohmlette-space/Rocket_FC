#include <Arduino.h>
#include <Adafruit_LSM6DSOX.h>
#include <SoftwareSerial.h> 
#include <TinyGPSPlus.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>


Adafruit_LSM6DSOX sox; 
Adafruit_BMP280 bmp;
SoftwareSerial ss(16, 17);
TinyGPSPlus gps;
void setup() {
    Serial.begin(115200);
    ss.begin(9600);
    while (!Serial) delay(10);

    if (!bmp.begin(0x76)) {
        Serial.println("Failed to find BME280 chip");
        while (1)
        delay(10);
    }

    if (!sox.begin_I2C()) {
        Serial.println("Failed to find LSM6DSOX chip");
        while (1) delay(10);

    }
    

    sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    sox.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

    Serial.println("LSM6DSOX Initialized!");
}

void loop() {
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&gyro, &accel, &temp);

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Gyro --- X: " + String(gyro.gyro.x) + " Y: " + String(gyro.gyro.y) + " Z: " + String(gyro.gyro.z));
    Serial.print("  |  ");
    Serial.print("Accel -- X: " + String(accel.acceleration.x) + " Y: " + String(accel.acceleration.y) + " Z: " + String(accel.acceleration.z));
    Serial.print("  |  ");
    Serial.println("Temp --- " + String(temp.temperature) + " deg C");

    delay(100);

    while(ss.available() > 0){
          gps.encode(ss.read());
        if (gps.location.isUpdated()){
            Serial.print("Latitude= "); 
            Serial.print(gps.location.lat(), 6);
            Serial.print(" Longitude= "); 
            Serial.println(gps.location.lng(), 6);
    }
        byte gpsData = ss.read();
        Serial.write(gpsData);

        Serial.print("Latitude= "); 
        Serial.print(gps.location.lat(), 6);
        Serial.print(" Longitude= "); 
        Serial.println(gps.location.lng(), 6);
    }
}