# ArduinoBleMouseProject

------------------------------------------------------------------------------------------------------------------------------------------------------
軟創畢業專題 - Graduation Project

專題初步構想
專題名稱：空鼠（由滑鼠名稱延伸）

模擬需求：老師在上課以簡報授課時，需使用滑鼠操控簡報，需使用雷射筆解說簡報，雖然現有簡報筆可解決此問題，但靈活性不夠其他操作還是需要使用滑鼠，且不可避免板書部份仍將使用粉筆手寫，如果投影設備與白板空間衝突（輔大進修部大樓教室），要板書必須重複的收起與降下投影布幕，線上教程的話則需使用滑鼠搭配書寫程式，便利性十分不足。

解決方案：開發手持物聯設備在空中模擬滑鼠，可以靈活的操作系統及簡報程式，開發可以線上書寫的頁面模擬白板，解決手持粉筆板書的不便，且每個學生連網皆可看到數位白板老師的書寫，線上教程的話便利性十足，一個設備解決了這些複雜的問題。

專題延伸：建立一個vr環境的白板，讓前面的成果應用在vr授課環境。（延伸未定案）

-------------------------------------------------------------------------------------------------------------------------------------------------------------
實施辦法

硬件:
開發版:ESP32
陀螺儀:MPU9250

語言:C++、Javascript、Html

技術：
MQTT 用戶端：用於從 IoT 設備接收資料。
WebSocket：用於即時將資料傳送到前端。
Node.js：伺服器端環境。

步驟：
利用陀螺儀接收位置資訊，再利用藍芽傳輸至電腦實現鼠標移動。
搭建 Node.js 伺服器：使用 Express 或其他 Node.js 框架。
接取 MQTT 用戶端：使用 MQTT.js 或相似庫訂閱 IoT 主題。
實作 WebSocket 通訊：使用 ws 或 Socket.IO 在伺服器與前端之間建立 WebSocket 連線。
中轉資料：將透過 MQTT 接收的資料透過 WebSocket 傳送到前端。

----------------------------------------------------------------------------------------------------------------------------------------------------------------
<img width="576" alt="未命名" src="https://github.com/511172176/ArduinoBleMouseProject/assets/151836005/81698cc7-fe0c-4499-b595-df9946b96099">

MQTT非即時傳輸 AsyncMqttClient.h https://github.com/marvinroger/async-mqtt-client
