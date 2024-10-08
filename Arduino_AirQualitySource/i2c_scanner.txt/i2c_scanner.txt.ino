#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>

// WiFi 설정
const char* ssid = "onion";
const char* password = "0323249817";

// NTP 서버 설정
const long utcOffsetInSeconds = 9 * 3600; // UTC+9 (KST)

// WiFi UDP 및 NTP 클라이언트 객체 생성
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

// BME680 객체 생성
Adafruit_BME680 bme; // I2C interface

// PMS5003 설정
SoftwareSerial pmsSerial(6, 7); // RX, TX

// MQ135 설정
const int mq135Pin = A2;  // MQ135 센서 핀

// MP503 설정
const int mp503Pin1 = 8;  // MP503 센서 A핀
const int mp503Pin2 = 9;  // MP503 센서 B핀

void setup() {
  // Serial 포트 초기화
  Serial.begin(9600);
  while (!Serial);

  // WiFi 연결 시도
  connectWiFi();

  // NTP 클라이언트 시작
  timeClient.begin();

  // BME680 센서 초기화
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);
  }

  // BME680 센서 설정
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320°C for 150 ms

  // PMS5003 센서 초기화
  pmsSerial.begin(9600);

  // MP503 센서 초기화
  pinMode(mp503Pin1, INPUT);
  pinMode(mp503Pin2, INPUT);

  // MQ135 센서 초기화
  pinMode(mq135Pin, INPUT);
}

void connectWiFi() {
  // WiFi 연결 시도
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // 연결 시도 중
    Serial.print(".");
    delay(1000);
  }
  
  // WiFi 연결 성공 시 정보 출력
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // NTP 시간 업데이트
  timeClient.update();

  // 현재 시간 가져오기
  String formattedTime = timeClient.getFormattedTime();

  // BME680에서 측정값 가져오기
  if (!bme.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  // 시리얼 모니터에 출력
  Serial.println("Present by Onion communications");
  Serial.print("Current time: ");
  Serial.println(formattedTime);
  Serial.println("--------------------------");
  Serial.println("BME680 sensor displays the measured value ->");
  Serial.print("Temperature: ");
  Serial.print(bme.temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(bme.humidity);
  Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(bme.pressure / 100.0);
  Serial.println(" hPa");

  Serial.print("Gas Resistance: ");
  Serial.print(bme.gas_resistance / 1000.0); // Convert to KOhms
  Serial.println(" KOhms");
  Serial.println("--------------------------");
  // Serial.println(); // 데이터 사이 구분을 위해 빈 줄 출력

  // PMS5003 데이터 읽기
  readPMSData();

  // MQ135 데이터 읽기
  readMQ135Data();

  // MP503 데이터 읽기
  readMP503Data();

  delay(5000); // 5초마다 측정값 출력
}

void readPMSData() {
  if (pmsSerial.available() > 0) {
    if (pmsSerial.find("\x42")) {
      int value = pmsSerial.read();  // Start character
      if (value == 0x4D) {
        int frameLength = pmsSerial.read() << 8 | pmsSerial.read(); // Read Frame length

        int pm1_0 = pmsSerial.read() << 8 | pmsSerial.read(); // PM1.0
        int pm2_5 = pmsSerial.read() << 8 | pmsSerial.read(); // PM2.5
        int pm10 = pmsSerial.read() << 8 | pmsSerial.read(); // PM10

        // Skip unused bytes
        for (int i = 0; i < 9; i++) {
          pmsSerial.read();
        }
        Serial.println("PMS5003 sensor displays the measured value ->");
        Serial.print("PM1.0: ");
        Serial.print(pm1_0);
        Serial.println(" µg/m³");

        Serial.print("PM2.5: ");
        Serial.print(pm2_5);
        Serial.println(" µg/m³");

        Serial.print("PM10: ");
        Serial.print(pm10);
        Serial.println(" µg/m³");

        Serial.println("--------------------------");
      }
    } else {
      // 데이터 읽기 실패 시 메시지 출력
      Serial.println("Failed to read from PMS5003 sensor!");
    }
  } else {
    // 센서로부터 데이터를 읽지 못한 경우
    Serial.println("No data available from PMS5003 sensor!");
  }
}

void readMQ135Data() {
  int sensorValue = analogRead(mq135Pin);
  float voltage = sensorValue * (5.0 / 1023.0);

  // 가스 농도 계산 (여기서는 예시로 대략적인 값을 사용)
  float co2 = calculateCO2(voltage);
  float nh3 = calculateNH3(voltage);
  float nox = calculateNOX(voltage);
  float alcohol = calculateAlcohol(voltage);
  float benzene = calculateBenzene(voltage);
  float smoke = calculateSmoke(voltage);

  // 시리얼 모니터에 출력
  Serial.println("MQ135 sensor displays the measured value ->");
  Serial.print("CO2: ");
  Serial.print(co2);
  Serial.println(" ppm");
  Serial.print("NH3: ");
  Serial.print(nh3);
  Serial.println(" ppm");
  Serial.print("NOx: ");
  Serial.print(nox);
  Serial.println(" ppm");
  Serial.print("Alcohol: ");
  Serial.print(alcohol);
  Serial.println(" ppm");
  Serial.print("Benzene: ");
  Serial.print(benzene);
  Serial.println(" ppm");
  Serial.print("Smoke: ");
  Serial.print(smoke);
  Serial.println(" ppm");
  Serial.println("--------------------------");
}

float calculateCO2(float voltage) {
  // CO2 농도 계산식 (예시)
  return voltage * 200.0;
}

float calculateNH3(float voltage) {
  // NH3 농도 계산식 (예시)
  return voltage * 150.0;
}

float calculateNOX(float voltage) {
  // NOx 농도 계산식 (예시)
  return voltage * 100.0;
}

float calculateAlcohol(float voltage) {
  // 알콜 농도 계산식 (예시)
  return voltage * 130.0;
}

float calculateBenzene(float voltage) {
  // 벤젠 농도 계산식 (예시)
  return voltage * 140.0;
}

float calculateSmoke(float voltage) {
  // 연기 농도 계산식 (예시)
  return voltage * 110.0;
}

void readMP503Data() {
  int sensorPin;
  int Pin1 = digitalRead(mp503Pin1);
  int Pin2 = digitalRead(mp503Pin2);

  if (Pin1 == 0 && Pin2 == 0){
    sensorPin = 0;
  }
  else if (Pin1 == 0 && Pin2 == 1){
    sensorPin = 1;
  }
  else if (Pin1 == 1 && Pin2 == 0){
    sensorPin = 2;
  }
  else if (Pin1 == 1 && Pin2 == 1){
    sensorPin = 3;
  }

  String gasType = getGasType(sensorPin);

  Serial.print("MP503 sensor displays the measured value ->");
  Serial.print("sensorPin: ");
  Serial.println(sensorPin);
  int airQuality = sensorPin;

  Serial.print("airQuality: ");
  Serial.print(airQuality);
  Serial.print(" | 검출된 가스: ");
  Serial.println(gasType);
  Serial.println("=====================================");
  Serial.println(); // 다음 출력 데이터와 구분하기 위해 
}

String getGasType(int sensorPin) {
  if (sensorPin == 0) return "없음, 좋은 공기";
  if (sensorPin <= 1) return "알코올 또는 연기";
  if (sensorPin <= 2) return "톨루엔 또는 아세톤";
  if (sensorPin <= 3) return "암모니아";

  return "위험한 공기";
}
