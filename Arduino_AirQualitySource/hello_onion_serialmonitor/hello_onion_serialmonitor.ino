#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C address 0x27, 16 column and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.begin(16, 2); // LCD 초기화
  lcd.backlight();  // 백라이트 켜기
  lcd.setCursor(0, 0); // 첫 번째 줄 첫 번째 칸에 커서 위치 설정
  lcd.print("Hello, World!"); // 메시지 출력
  
  delay(5000); // 5초 동안 백라이트 켜진 상태로 유지
  lcd.noBacklight(); // 백라이트 끄기
}

void loop() {
  // 아무 작업도 하지 않음
}
