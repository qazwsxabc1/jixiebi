#include <ESP8266WiFi.h>
#include <espnow.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

uint8_t broadcastAddress[] = {0x8C,0xAA,0xB5,0x0C,0xC6,0x47};//定义发送方MAC地址
typedef struct {
int16_t ax;  // 加速度X轴
int16_t ay;  // 加速度Y轴
int16_t az;  // 加速度Z轴
int16_t gx;  // 陀螺仪X轴
int16_t gy;  // 陀螺仪Y轴
int16_t gz;  // 陀螺Z轴
} MotionData;

MotionData myData; // 与发送方一致的结构体

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(MotionData)); // 解析接收到的数据
  // 将数据转化为JSON并通过串口发送到UNO
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["ax"] = myData.ax;
  jsonDoc["ay"] = myData.ay;
  jsonDoc["az"] = myData.az;
  jsonDoc["gx"] = myData.gx;
  jsonDoc["gy"] = myData.gy;
  jsonDoc["gz"] = myData.gz;
  String jsonString;
  serializeJson(jsonDoc, jsonString);
  Serial.println(jsonString); // 通过串口发送JSON数据
  Serial.println(); // 添加换行符，以便于UNO端解析
}
void setup() {
  Serial.begin(115200); // 与UNO串口通信速度匹配
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_CONTROLLER, 1, NULL, 0); // 添加主ESP的MAC作为对端
}
void loop() {
  // 空循环保持接收ESP-NOW数据
}
