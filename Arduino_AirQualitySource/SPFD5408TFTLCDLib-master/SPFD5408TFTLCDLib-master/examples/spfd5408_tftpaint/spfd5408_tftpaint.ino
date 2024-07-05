#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library

// 핀 설정 (필요에 따라 수정)
#define TFT_CS     10
#define TFT_RST    9
#define TFT_DC     8

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(9600);
  Serial.println("Hello, World!");

  tft.begin();
  tft.setRotation(3); // 필요한 경우 회전 설정
  tft.fillScreen(ILI9341_BLACK);
  
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  
  tft.setTextSize(2);
  tft.println("Hello, World!");
}

void loop() {
  // 메인 루프에서 할 작업 없음
}
