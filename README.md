### 軟創畢業專題 - 空鼠

#### 專題初步構想
##### 專題名稱：
空鼠（由滑鼠名稱延伸）

##### 模擬需求：
在上課簡報或線上教程時，老師需使用滑鼠操作系統及簡報程式，且可能需要板書，但現有的簡報筆靈活性不夠，且板書部分仍需使用傳統粉筆，操作不便。

##### 解決方案：
開發一種手持的物聯網設備，在空中模擬滑鼠操作，同時開發可線上書寫的頁面模擬白板，解決板書的不便，提高簡報和教學的便利性。

##### 專題延伸：
建立一個VR環境的白板，將前述成果應用於VR授課環境。

#### 實施辦法
##### 硬件：
- 開發板：ESP32
- 陀螺儀：MPU9250

##### 語言：
- C++
- JavaScript
- HTML

##### 技術：
- **MQTT 用戶端**：用於從 IoT 設備接收資料。
- **WebSocket**：用於即時將資料傳送到前端。
- **Node.js**：伺服器端環境。

##### 步驟：
1. 利用陀螺儀接收位置資訊，再利用藍芽傳輸至電腦實現鼠標移動。
2. 搭建 Node.js 伺服器：使用 Express 或其他 Node.js 框架。
3. 接取 MQTT 用戶端：使用 MQTT.js 或相似庫訂閱 IoT 主題。
4. 實作 WebSocket 通訊：使用 ws 或 Socket.IO 在伺服器與前端之間建立 WebSocket 連線。
5. 中轉資料：將透過 MQTT 接收的資料透過 WebSocket 傳送到前端。

#### 資源連結
- MQTT非即時傳輸：[AsyncMqttClient.h](https://github.com/marvinroger/async-mqtt-client)

#### 備註
- 使用Markdown風格撰寫，以清晰展示專題構想及實施辦法。
 *** 
<img width="576" alt="未命名" src="https://github.com/511172176/ArduinoBleMouseProject/assets/151836005/81698cc7-fe0c-4499-b595-df9946b96099">

