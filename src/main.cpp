#include <Arduino.h>
#include <Adafruit_LSM6DSOX.h>

Adafruit_LSM6DSOX sox; //

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10);

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

    Serial.print("Gyro --- X: " + String(gyro.gyro.x) + " Y: " + String(gyro.gyro.y) + " Z: " + String(gyro.gyro.z));
    Serial.print("  |  ");
    Serial.print("Accel -- X: " + String(accel.acceleration.x) + " Y: " + String(accel.acceleration.y) + " Z: " + String(accel.acceleration.z));
    Serial.print("  |  ");
    Serial.println("Temp --- " + String(temp.temperature) + " deg C");

    delay(100);
}
