// BLE central device that creates a server to connect to the phone and
// Scans for a accelerometer service

#include <ArduinoBLE.h>

#define SERVICE_UUID "3b0814b8-e6ab-11ed-a05b-0242ac120003"
#define CHARACTERISTIC_UUID "333ad260-e6ac-11ed-a05b-0242ac120003"

// This is the uuid for the remote band
#define ACCEL_UUID "0b03321e-e6ad-11ed-a05b-0242ac120003"
#define ACCEL_CHAR_UUID "1a01ecfc-e6b1-11ed-a05b-0242ac120003"

// #define MAX_PERIPHERALS 2
#define CHAR_SIZE 128

char print_str[CHAR_SIZE];

BLEService exerciseService(SERVICE_UUID);
BLEStringCharacteristic progressCharacteristic(CHARACTERISTIC_UUID, BLERead | BLENotify, CHAR_SIZE);

BLEDevice peripheral;
BLECharacteristic ble_char;

BLEDevice central;

// This initializes the BLE server for the phone to connect to
void initBLE() {
  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  BLE.setLocalName("Mcpy (central device)");
  BLE.setAdvertisedService(exerciseService);
  exerciseService.addCharacteristic(progressCharacteristic);
  BLE.addService(exerciseService);
  progressCharacteristic.setValue("<(O_O)>");

  Serial.println("Mcpy central device started");

  BLE.advertise();

  Serial.print("Server address: ");
  Serial.println(BLE.address().c_str());
}

void connectToCentral() {
  central = BLE.central();

  if (central) {
    Serial.print("Connected to central MAC: ");
    Serial.println(central.address());
  }
}

void connectToPeripheral() {
  BLEDevice peripheral;

  Serial.println("- Discovering peripheral device...");

  do {
    BLE.scanForUuid(ACCEL_UUID);
    peripheral = BLE.available();
    Serial.print(".");
    delay(100);
  } while (!peripheral);

  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device Name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println();
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connect to peripheral device!");
    Serial.println();
  } else {
    Serial.println("* Concnection to peripheral device failed!");
    Serial.println();
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println();
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println();
    peripheral.disconnect();
    return;
  }

  BLECharacteristic characteristic = peripheral.characteristic(ACCEL_CHAR_UUID);

  if (!characteristic) {
    Serial.println("* Peripheral device does not have string_type characteristic!");
    peripheral.disconnect();
    return;
  } else if (!characteristic.canSubscribe()) {
    Serial.println("* Periheral device does not have a subscribable string_type characteristic!");
    peripheral.disconnect();
    return;
  }

  while (peripheral.connected()) {
    central = BLE.central();
    characteristic.readValue(print_str, CHAR_SIZE);
    Serial.print(print_str);
    if (central.connected()) {
      Serial.print(" HEHEHE");
      progressCharacteristic.setValue(print_str);
    }
    Serial.println();
  }
}

// Arduino Functions \/ ------------------------------------------ \/
 
void setup() {
  Serial.begin(9600);
  while (!Serial);

  initBLE();
  connectToCentral();
}

void loop() {
  connectToPeripheral();
}