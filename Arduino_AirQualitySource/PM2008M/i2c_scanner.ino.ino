#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("I2C 스캐너 시작...");
}

void loop() {
  byte error, address;
  int nDevices;

  Serial.println("장치를 검색하는 중...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C 장치 발견 주소: 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);

      nDevices++;
    } else if (error==4) {
      Serial.print("알 수 없는 오류 주소: 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("I2C 장치를 찾을 수 없음.\n");
  else
    Serial.println("검색 완료.\n");

  delay(5000); // 5초 간격으로 검색
}
