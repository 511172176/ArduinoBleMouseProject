#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <BleMouse.h>

#define AVG_SAMPLES 2 // 減少平均樣本數量以提高反應速度
#define TOUCH_PIN T9  // 定義觸摸引腳為T9，實際上對應的是GPIO 32 pin 12

int gyroXAvg = 0;
int gyroZAvg = 0;
int gyroXSamples[AVG_SAMPLES];
int gyroZSamples[AVG_SAMPLES];
int sampleIndex = 0;
float gyroXFiltered = 0;
float gyroZFiltered = 0;
const float filterAlpha = 0.2; // 調整濾波係數以提高靈敏度

// 在全域範圍內聲明i2cData陣列
uint8_t i2cData[14];
int16_t gyroX, gyroZ;

int Sensitivity = 400; // 減小靈敏度值以提高滑鼠移動速度
int delayi = 20;

BleMouse bleMouse;
const uint8_t IMUAddress = 0x68;
const uint16_t I2C_TIMEOUT = 1000;
bool touchPressed = false; // 用於追蹤觸摸狀態的變量

void addSample(int gyroX, int gyroZ) {
    gyroXSamples[sampleIndex] = gyroX;
    gyroZSamples[sampleIndex] = gyroZ;
    sampleIndex = (sampleIndex + 1) % AVG_SAMPLES;

    gyroXAvg = 0;
    gyroZAvg = 0;
    for (int i = 0; i < AVG_SAMPLES; i++) {
        gyroXAvg += gyroXSamples[i];
        gyroZAvg += gyroZSamples[i];
    }
    gyroXAvg /= AVG_SAMPLES;
    gyroZAvg /= AVG_SAMPLES;
}

uint8_t i2cWrite(uint8_t registerAddress, uint8_t* data, uint8_t length, bool sendStop) {
    Wire.beginTransmission(IMUAddress);
    Wire.write(registerAddress);
    Wire.write(data, length);
    return Wire.endTransmission(sendStop); // 回傳0為成功
}

uint8_t i2cWrite2(uint8_t registerAddress, uint8_t data, bool sendStop) {
    return i2cWrite(registerAddress, &data, 1, sendStop); // 回傳0為成功
}

uint8_t i2cRead(uint8_t registerAddress, uint8_t* data, uint8_t nbytes) {
    Wire.beginTransmission(IMUAddress);
    Wire.write(registerAddress);
    if(Wire.endTransmission(false) != 0) return 1; // 結束傳輸，如果失敗則回傳1
    Wire.requestFrom(IMUAddress, nbytes);
    for(uint8_t i = 0; i < nbytes; i++) {
        if(!Wire.available()) return 2; // 如果無可用資料則回傳2
        data[i] = Wire.read();
    }
    return 0; // 讀取成功
}

void setup() {
    Wire.begin();
    Serial.begin(115200);
    bleMouse.begin();
}

void loop() {
    // 讀取觸摸引腳的值
    int touchValue = touchRead(TOUCH_PIN);
    // 檢測觸摸按下
    if (touchValue < 40 && !touchPressed) {
        Serial.println("觸摸偵測到，模擬滑鼠按住不放");
        bleMouse.press(MOUSE_LEFT); // 模擬滑鼠左鍵按住不放
        touchPressed = true;
    }
    // 檢測觸摸釋放
    else if (touchValue >= 40 && touchPressed) {
        Serial.println("觸摸釋放，模擬滑鼠釋放");
        bleMouse.release(MOUSE_LEFT); // 模擬滑鼠左鍵釋放
        touchPressed = false;
    }

    if(i2cRead(0x3B, i2cData, 14) != 0) {
        return; // 如果讀取失敗，則直接返回
    }

    gyroX = ((i2cData[8] << 8) | i2cData[9]);
    gyroZ = ((i2cData[12] << 8) | i2cData[13]);

    gyroX = gyroX / Sensitivity / 1.1 * -1;
    gyroZ = gyroZ / Sensitivity * -1;

    // 添加樣本到滑動平均緩衝區
    addSample(gyroX, gyroZ);

    if(bleMouse.isConnected()){
        Serial.print("X 平均值: "); Serial.print(gyroXAvg);
        Serial.print("   Z 平均值: "); Serial.println(gyroZAvg);
        bleMouse.move(gyroZAvg, -gyroXAvg); // 使用平均值進行移動
    }

    delay(delayi); // 根據需要調整延時以優化性能
}
