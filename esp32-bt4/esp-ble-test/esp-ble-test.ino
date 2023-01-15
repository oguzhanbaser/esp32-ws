#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_LED "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_ADC "5ed9c888-7bf7-11ed-a1eb-0242ac120002"


// pin definitions
const uint8_t led1_pin = 19;
const uint8_t led2_pin = 18;
const uint8_t led3_pin = 5;

const uint8_t led_arr[] = { led1_pin, led2_pin, led3_pin };

const uint8_t btn1_pin = 34;
const uint8_t btn2_pin = 35;

const uint8_t pot_pin = 32;

// variables
unsigned long lastTime = 0;
uint16_t adcVal = 0;
bool deviceConnected = false;
BLECharacteristic *pCharacteristicLED, *pCharacteristicADC;

// BLE Class callbacks
class MyServerCallback : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
    Serial.println("Device connected");
  }

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    Serial.println("Device Disconnected");
    BLEDevice::startAdvertising();
  }
};

// BLE Characteristic Callbacks
class MyCharacteristicsCallbacks : public BLECharacteristicCallbacks {

  void onWrite(BLECharacteristic *pCharacteristics) {
    if (pCharacteristics == pCharacteristicLED) {

      std::string value = pCharacteristics->getValue();
      String ss = value.c_str();

      int indx1 = ss.indexOf("|");
      String str1 = ss.substring(0, indx1);
      int indx2 = ss.indexOf("?");
      String str2 = ss.substring(indx1 + 1, indx2);
      Serial.printf("Value: %s\n%s %s\n", ss, str1, str2);

      int val1 = str1.toInt();
      int val2 = str2.toInt();

      digitalWrite(led_arr[val1], val2);
    }
  }
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(led3_pin, OUTPUT);

  pinMode(btn1_pin, INPUT);
  pinMode(btn2_pin, INPUT);

  digitalWrite(led1_pin, LOW);
  digitalWrite(led2_pin, LOW);
  digitalWrite(led3_pin, LOW);

  // BLE Device Name
  BLEDevice::init("My BLE ESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallback());  // set BLE server callbacks

  // create BLE service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create Service Characteristic for LED status
  pCharacteristicLED = pService->createCharacteristic(
    CHARACTERISTIC_UUID_LED,
    BLECharacteristic::PROPERTY_WRITE);
  pCharacteristicLED->setCallbacks(new MyCharacteristicsCallbacks());  // create LED characteristic callback funciton

  pCharacteristicADC = pService->createCharacteristic(
    CHARACTERISTIC_UUID_ADC,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);

  // add notify spec for BLE device
  pCharacteristicADC->addDescriptor(new BLE2902());

  // start BLE service
  pService->start();

  // create BLE advertising object
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);  // add service UUDI
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();       // start BLE advertising
}

void loop() {
  // put your main code here, to run repeatedly:

  // read ADC value in every 200ms
  if (millis() - lastTime > 200) {
    adcVal = analogRead(pot_pin);  // read pot value with ADC

    // if BLE device connected set data to BLE characteristic and notify connected device
    if (deviceConnected) {
      pCharacteristicADC->setValue((uint8_t *)&adcVal, 2);

      pCharacteristicADC->notify();
    }

    lastTime = millis();
  }
}
