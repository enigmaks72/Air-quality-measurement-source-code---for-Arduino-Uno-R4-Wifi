#include <Wire.h>  // I2C 통신을 위한 Wire 라이브러리 포함

#define PM2008M_ADDRESS 0x28  // I2C 주소 (0x28)

void setup() {
  Wire.begin();  // I2C 통신 시작
  Serial.begin(9600);  // 시리얼 모니터 시작
  Serial.println("PM2008M I2C 데이터 읽기 시작...");
}

void loop() {
  Wire.beginTransmission(PM2008M_ADDRESS);
  Wire.write(0x00);  // 데이터 요청 명령 (예시, 실제 명령은 데이터시트 참조)
  Wire.endTransmission();
  
  Wire.requestFrom(PM2008M_ADDRESS, 32);
  
  if (Wire.available() == 32) {
    byte buffer[32];
    for (int i = 0; i < 32; i++) {
      buffer[i] = Wire.read();
    }

    int pm1_0 = (buffer[10] << 8) | buffer[11];
    int pm2_5 = (buffer[12] << 8) | buffer[13];
    int pm10 = (buffer[14] << 8) | buffer[15];

    Serial.print("PM1.0: ");
    Serial.print(pm1_0);
    Serial.print(" µg/m³, ");
    Serial.print("PM2.5: ");
    Serial.print(pm2_5);
    Serial.print(" µg/m³, ");
    Serial.print("PM10: ");
    Serial.println(pm10);
  } else {
    Serial.println("데이터 읽기 실패.");
  }

  delay(3000);  // 1초 간격으로 데이터 읽기
}
