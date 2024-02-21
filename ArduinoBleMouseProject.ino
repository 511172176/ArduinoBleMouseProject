#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <BleMouse.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define AVG_SAMPLES 2 // 減少平均樣本數量以提高反應速度
#define TOUCH_PIN T9  // 定義觸摸引腳為T9，實際上對應的是GPIO 32 pin 12

// WiFi 參數
const char* ssid = "HITRON-57F0-2.4G";     // 這裡替換成你的 Wi-Fi SSID
const char* password = "0229328386"; // 這裡替換成你的 Wi-Fi 密碼

// MQTT 伺服器參數
const char* mqtt_server = "192.168.0.196"; // 這裡替換成你的 MQTT 伺服器地址
const char* topic = "test/topic"; // 設置MQTT主題
const int mqtt_port = 1883; // MQTT使用的端口號，標準端口為1883

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
WiFiClient espClient;
PubSubClient client(espClient);

const uint8_t IMUAddress = 0x68;
const uint16_t I2C_TIMEOUT = 1000;
bool touchPressed = false; // 用於追蹤觸摸狀態的變量

void setup_wifi() {
  //Serial.begin(115200); // 開始串列通訊
  Serial.println("Connecting to WiFi..."); // 打印正在連接Wi-Fi
  WiFi.begin(ssid, password); // 使用Wi-Fi名稱和密碼連接

  while (WiFi.status() != WL_CONNECTED) { // 等待直到連接成功
    delay(500); // 延遲500毫秒
    Serial.print("."); // 打印進度
  }

  Serial.println("");
  Serial.println("Connected to WiFi!"); // 打印Wi-Fi連接成功
  Serial.println("IP address: "); // 打印IP地址
  Serial.println(WiFi.localIP()); // 獲取並打印IP地址
}

void reconnect() {
  while (!client.connected()) { // 當未連接到MQTT伺服器時
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP32Client")) { // 嘗試連接到MQTT伺服器
      Serial.println("Connected to MQTT Broker!"); // 連接成功
      client.subscribe("test/topic"); // 訂閱測試主題
    } else {
      Serial.print("Failed, rc="); // 連接失敗
      Serial.print(client.state()); // 打印連接狀態
      Serial.println(" try again in 5 seconds"); // 5秒後重試
      delay(5000); // 延遲5秒
    }
  }
}

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
    setup_wifi(); // 執行Wi-Fi設置函數
    //WiFi.begin(ssid, password);
    client.setServer(mqtt_server, mqtt_port); // 設置MQTT伺服器和端口
}

void loop() {
    if (!client.connected()) { // 如果未連接到MQTT伺服器
    reconnect(); // 執行重連函數
    }
    client.loop(); // 維持MQTT連接*/
    
    // 讀取觸摸引腳的值
    int touchValue = touchRead(TOUCH_PIN);
    // 檢測觸摸按下
    if (touchValue < 40 && !touchPressed) {
        //Serial.println("觸摸偵測到，模擬滑鼠按住不放");
        bleMouse.press(MOUSE_LEFT); // 模擬滑鼠左鍵按住不放
        touchPressed = true;
    }
    // 檢測觸摸釋放
    else if (touchValue >= 40 && touchPressed) {
        //Serial.println("觸摸釋放，模擬滑鼠釋放");
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
        //Serial.print("X 平均值: "); Serial.print(gyroXAvg);
        //Serial.print("   Z 平均值: "); Serial.println(gyroZAvg);
        
        char attributes[100];

        // 將數值和文字組合成字串，儲存在char數組中
        // 確保char數組的大小足夠大以儲存完整的訊息
        snprintf(attributes, sizeof(attributes), "X: %d   Z: %d Touch: %d", gyroXAvg, gyroZAvg, touchPressed? 1:0);

        // 發布消息到MQTT主題
        client.publish(topic, attributes);
        
        bleMouse.move(gyroZAvg, -gyroXAvg); // 使用平均值進行移動
    }

    delay(delayi); // 根據需要調整延時以優化性能
}