#include <Wire.h>
#include <MPU6050.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// 定义接收方MAC地址
uint8_t broadcastAddress[] = {0x48,0x3F,0xDA,0x49,0x0A,0xA6};

//定义结构体
typedef struct {
  int16_t ax;  // 加速度X轴
  int16_t ay;  // 加速度Y轴
  int16_t az;  // 加速度Z轴
  int16_t gx;  // 陀螺仪X轴
  int16_t gy;  // 陀螺仪Y轴
  int16_t gz;  // 陀螺仪Z轴
} MotionData;

MPU6050 mpuSensor;//实体化mpu6050
MotionData sensorData;//实体化结构体

//回调函数
void OnDataSent(uint8_t *mac_addr, uint8_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  if (status == 0) {
    Serial.println("Delivery Success :");
  } else {
    Serial.println("Delivery Fail :");
  }
}
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  //初始化esp_now
   if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Wire.begin();// 初始化I2C总线
  mpuSensor.initialize();// 初始化MPU6050并检查
  // 设置ESP8266为控制器角色
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  // 注册发送回调函数，直接传递函数指针
  esp_now_register_send_cb(OnDataSent);
  // 配对接收设备，调整参数类型
  esp_now_add_peer((uint8_t*)broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
void loop() {
  mpuSensor.getMotion6(&sensorData.ax, &sensorData.ay, &sensorData.az, 
                     &sensorData.gx, &sensorData.gy, &sensorData.gz);
  esp_now_send((uint8_t*)broadcastAddress, (uint8_t *)&sensorData, sizeof(MotionData));
  delay(100);
}
