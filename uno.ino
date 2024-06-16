#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <Servo.h>

SoftwareSerial espSerial(2, 3); // RX, TX 连接到ESP8266接收端的TX, RX
Servo servoX;
Servo servoY;
Servo servoZ;

// 结构体定义
typedef struct {
  int16_t ax;  // 加速度X轴
  int16_t ay;  // 加速度Y轴
  int16_t az;  // 加速度Z轴
  int16_t gx;  // 陀螺仪X轴
  int16_t gy;  // 陀螺仪Y轴
  int16_t gz;  // 陀螺仪Z轴
} MotionData;

void setup() {
  Serial.begin(115200); // 保持与ESP8266端的波特率一致，用于调试输出
  espSerial.begin(115200); // 确保与ESP8266的波特率一致
  servoX.attach(9, 500, 2000);
  servoY.attach(10, 500, 2000);
  servoZ.attach(11, 500, 2000);
}

void loop() {
  if (espSerial.available()) {
    String jsonData = espSerial.readStringUntil('\n'); // 读取一行数据
    if (jsonData.length() == 0) {
      Serial.println("No data received.");
    }
    if (jsonData.length() > 0) { // 检查是否有数据读取
      DynamicJsonDocument jsonDoc(200); // 动态分配大小以适应数据
      DeserializationError error = deserializeJson(jsonDoc, jsonData);
      if (!error) {
        MotionData myData;
        myData.ax = jsonDoc["ax"].as<int16_t>();
        myData.ay = jsonDoc["ay"].as<int16_t>();
        myData.az = jsonDoc["az"].as<int16_t>();
        myData.gx = jsonDoc["gx"].as<int16_t>(); // ESP8266也发送陀螺仪数据
        myData.gy = jsonDoc["gy"].as<int16_t>();
        myData.gz = jsonDoc["gz"].as<int16_t>();
        // 映射关系和舵机控制逻辑
        float value1 = map(myData.ax, -17000, 17000, 0, 180);
        float value2 = map(myData.ay, -17000, 17000, 0, 180);
        float value3 = map(myData.az, -17000, 17000, 0, 180);
        servoX.write(value1); // 控制X轴舵机
        servoY.write(value2); // 控制Y轴舵机
        servoZ.write(value3); // 控制Z轴舵机
        Serial.print("Servo positions: ");
        Serial.print(value1);
        Serial.print("|");
        Serial.print(value2);
        Serial.print("|");
        Serial.println(value3);
      } else {
        Serial.println("deserializeJson() failed");
      }
    }
  }
  delay(10); 
}
