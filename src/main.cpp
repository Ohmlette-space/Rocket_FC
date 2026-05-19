#include <Arduino.h>
#include <Adafruit_LSM6DSOX.h>
#include <SoftwareSerial.h> 
#include <TinyGPSPlus.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FRAM_I2C.h>
#include <Wire.h>

#define FRAM_SIZE 32768

Adafruit_LSM6DSOX sox;
Adafruit_LSM6DSOX sox_2; 
Adafruit_BMP280 bmp;
Adafruit_FRAM_I2C fram;
SoftwareSerial ss(16, 17);
TinyGPSPlus gps;

int rstPin = 36;
int logPin = 13;

int rst = false;
int log_ = false;

int indexFRAM = 0;

void writeByte(uint8_t value) {
    fram.write(indexFRAM, &value, 1);
    indexFRAM ++;
}

void writeBytes(uint8_t *values, size_t len) {
    fram.write(indexFRAM, values, len);
    indexFRAM ++;
}

void writeFloat(float value) {
    uint8_t *ptr = (uint8_t *)&value;
    fram.write(indexFRAM, ptr, 4);
    indexFRAM += 4;
}

void readByte(uint16_t addr, uint8_t *value) {
    fram.read(addr, value, 1);
}

void readFloat(uint16_t addr, float *value) {
    uint8_t buffer[4];
    fram.read(addr, buffer, 4);
    memcpy((void *)value, buffer, 4);
}

void writeSensorsData(float g_x_a, float g_y_a, float g_z_a, float a_x_a, float a_y_a, float a_z_a,
    float g_x_b, float g_y_b, float g_z_b, float a_x_b, float a_y_b, float a_z_b,
    float temp, float pressure) {

    writeFloat(g_x_a);
    writeFloat(g_y_a);
    writeFloat(g_z_a);
    writeFloat(a_x_a);
    writeFloat(a_y_a);
    writeFloat(a_z_a);
    writeFloat(g_x_b);
    writeFloat(g_y_b);
    writeFloat(g_z_b);
    writeFloat(a_x_b);
    writeFloat(a_y_b);
    writeFloat(a_z_b);
    writeFloat(temp);
    writeFloat(pressure);
}

void readSensorsData(uint16_t addr, float *g_x_a, float *g_y_a, float *g_z_a, float *a_x_a, float *a_y_a, float *a_z_a,
    float *g_x_b, float *g_y_b, float *g_z_b, float *a_x_b, float *a_y_b, float *a_z_b, float *temp, float *pressure) {

    readFloat(addr + 0x00, g_x_a);
    readFloat(addr + 0x04, g_y_a);
    readFloat(addr + 0x08, g_z_a);
    readFloat(addr + 0x0C, a_x_a);
    readFloat(addr + 0x10, a_y_a);
    readFloat(addr + 0x14, a_z_a);
    readFloat(addr + 0x18, g_x_b);
    readFloat(addr + 0x1C, g_y_b);
    readFloat(addr + 0x20, g_z_b);
    readFloat(addr + 0x24, a_x_b);
    readFloat(addr + 0x28, a_y_b);
    readFloat(addr + 0x2C, a_z_b);
    readFloat(addr + 0x30, temp);
    readFloat(addr + 0x34, pressure);

    printf("Gyro (A): X=%f Y=%f Z=%f\n", *g_x_a, *g_y_a, *g_z_a);
    printf("Accel (A): X=%f Y=%f Z=%f\n", *a_x_a, *a_y_a, *a_z_a);
    printf("Gyro (B): X=%f Y=%f Z=%f\n", *g_x_b, *g_y_b, *g_z_b);
    printf("Accel (B): X=%f Y=%f Z=%f\n", *a_x_b, *a_y_b, *a_z_b);
    printf("Temp: %f\n", *temp);
    printf("Pressure: %f\n", *pressure);
}

void setup() {
    Serial.begin(115200);
    ss.begin(9600);
    while (!Serial) delay(10);

    if (!bmp.begin(0x76)) {
        Serial.println("Failed to find BME280 chip");
        while (1)
        delay(10);
    }

    if (!sox.begin_I2C(0x6A)) {
        Serial.println("Failed to find LSM6DSOX chip");
        while (1) delay(10);
    }

    if (!sox_2.begin_I2C(0x6B)) {
        Serial.println("Failed to find LSM6DSOX 2 chip");
        while (1) delay(10);
    }

    if (!fram.begin(0x50))
    {
        Serial.println("Failed to find FRAM chip");
        while (1) delay(10);
    }
    
    pinMode(rstPin, INPUT);
    pinMode(logPin, INPUT);

    sox.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    sox.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    sox.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    sox.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

    sox_2.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
    sox_2.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
    sox_2.setAccelDataRate(LSM6DS_RATE_12_5_HZ);
    sox_2.setGyroDataRate(LSM6DS_RATE_12_5_HZ);

    Serial.println("LSM6DSOX Initialized!");

    Serial.println("Writing to address 0x00");
    writeFloat(55.5);
}

void clearFRAM() {
    for (uint16_t addr = 0; addr < FRAM_SIZE; addr++) {
        uint8_t buffer[4] = {0};
        fram.write(addr, buffer, 4);
    }

  Serial.println("Done.");
}

void logFRAM() {
    /*
    for (uint16_t addr = 0; addr < C; addr++) {
    uint8_t b;
    uint8_t buffer[1];

    fram.read(addr, buffer, 1);
    memcpy((void *)&b, buffer, 1);
    Serial.print(addr, HEX);
    Serial.print(": ");
    Serial.println(b);
    }
    */

    float g_x_a, g_y_a, g_z_a, a_x_a, a_y_a, a_z_a, g_x_b, g_y_b, g_z_b, a_x_b, a_y_b, a_z_b, temp, pressure;

    for (int i = 0; i < FRAM_SIZE; i += 56)
    {
        readSensorsData(i, &g_x_a, &g_y_a, &g_z_a, &a_x_a, &a_y_a, &a_z_a,
            &g_x_b, &g_y_b, &g_z_b, &a_x_b, &a_y_b, &a_z_b, &temp, &pressure);
    }
    

    Serial.println("Done.");
}

void loop() {

    rst = digitalRead(rstPin);
    log_ = digitalRead(logPin);

    if (rst == 1)
    {
        clearFRAM();
    }
    if (log_ == 1)
    {
        logFRAM();
    }
    

    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    sox.getEvent(&gyro, &accel, &temp);

    sensors_event_t accel_2;
    sensors_event_t gyro_2;
    sensors_event_t temp_2;
    sox_2.getEvent(&gyro_2, &accel_2, &temp_2);

    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("[1] Gyro --- X: " + String(gyro.gyro.x) + " Y: " + String(gyro.gyro.y) + " Z: " + String(gyro.gyro.z));
    Serial.print("  |  ");
    Serial.print("Accel -- X: " + String(accel.acceleration.x) + " Y: " + String(accel.acceleration.y) + " Z: " + String(accel.acceleration.z));
    Serial.print("  |  ");
    Serial.println("Temp --- " + String(temp.temperature) + " deg C");

    Serial.print("[2] Gyro --- X: " + String(gyro_2.gyro.x) + " Y: " + String(gyro_2.gyro.y) + " Z: " + String(gyro_2.gyro.z));
    Serial.print("  |  ");
    Serial.println("Accel -- X: " + String(accel_2.acceleration.x) + " Y: " + String(accel_2.acceleration.y) + " Z: " + String(accel_2.acceleration.z));

    writeSensorsData(gyro.gyro.x, gyro.gyro.y, gyro.gyro.z, accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
        gyro_2.gyro.x, gyro_2.gyro.y, gyro_2.gyro.z, accel_2.acceleration.x, accel_2.acceleration.y, accel_2.acceleration.z,
        temp.temperature, bmp.readPressure());

    delay(100);

    while(ss.available() > 0){
        byte gpsData = ss.read();
        gps.encode(gpsData);
        if (gps.location.isUpdated()){
            Serial.print("Latitude= "); 
            Serial.print(gps.location.lat(), 6);
            Serial.print(" Longitude= "); 
            Serial.println(gps.location.lng(), 6);
    }
        Serial.write(gpsData);

        Serial.print("Latitude= "); 
        Serial.print(gps.location.lat(), 6);
        Serial.print(" Longitude= "); 
        Serial.println(gps.location.lng(), 6);
    }

    float b;
    readFloat(0x00, &b);
    Serial.print("Read back float value: ");
    Serial.println(b);
}