#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

#define TFT_CS     7  // Define your TFT chip select pin
#define TFT_RST    40   // Define your TFT reset pin
#define TFT_DC     39   // Define your TFT data/command pin

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int scanTime = 5; // duration to scan for BLE devices in seconds
BLEScan* pBLEScan;

std::map<std::string, std::string> sigUUIDs = {
    {"00001800-0000-1000-8000-00805f9b34fb", "Generic Access"},
    {"00001801-0000-1000-8000-00805f9b34fb", "Generic Attribute"},
    {"00001802-0000-1000-8000-00805f9b34fb", "Immediate Alert"},
    {"00001803-0000-1000-8000-00805f9b34fb", "Link Loss"},
    {"00001804-0000-1000-8000-00805f9b34fb", "Tx Power"},
    {"00001805-0000-1000-8000-00805f9b34fb", "Current Time Service"},
    {"00001806-0000-1000-8000-00805f9b34fb", "Reference Time Update Service"},
    {"00001807-0000-1000-8000-00805f9b34fb", "Next DST Change Service"},
    {"00001808-0000-1000-8000-00805f9b34fb", "Glucose"},
    {"00001809-0000-1000-8000-00805f9b34fb", "Health Thermometer"},
    {"0000180A-0000-1000-8000-00805f9b34fb", "Device Information"},
    {"0000180B-0000-1000-8000-00805f9b34fb", "Network Availability"},
    {"0000180C-0000-1000-8000-00805f9b34fb", "Watchdog"},
    {"0000180D-0000-1000-8000-00805f9b34fb", "Heart Rate"},
    {"0000180E-0000-1000-8000-00805f9b34fb", "Phone Alert Status Service"},
    {"0000180F-0000-1000-8000-00805f9b34fb", "Battery Service"},
    {"00001810-0000-1000-8000-00805f9b34fb", "Blood Pressure"},
    {"00001811-0000-1000-8000-00805f9b34fb", "Alert Notification Service"},
    {"00001812-0000-1000-8000-00805f9b34fb", "Human Interface Device"},
    {"00001813-0000-1000-8000-00805f9b34fb", "Scan Parameters"},
    {"00001814-0000-1000-8000-00805f9b34fb", "Running Speed and Cadence"},
    {"00001815-0000-1000-8000-00805f9b34fb", "Automation IO"},
    {"00001816-0000-1000-8000-00805f9b34fb", "Cycling Speed and Cadence"},
    {"00001818-0000-1000-8000-00805f9b34fb", "Cycling Power"},
    {"00001819-0000-1000-8000-00805f9b34fb", "Location and Navigation"},
    {"0000181A-0000-1000-8000-00805f9b34fb", "Environmental Sensing"},
    {"0000181B-0000-1000-8000-00805f9b34fb", "Body Composition"},
    {"0000181C-0000-1000-8000-00805f9b34fb", "User Data"},
    {"0000181D-0000-1000-8000-00805f9b34fb", "Weight Scale"},
    {"0000181E-0000-1000-8000-00805f9b34fb", "Bond Management"},
    {"0000181F-0000-1000-8000-00805f9b34fb", "Continuous Glucose Monitoring"},
    {"00001820-0000-1000-8000-00805f9b34fb", "Internet Protocol Support"},
    {"00001821-0000-1000-8000-00805f9b34fb", "Indoor Positioning"},
    {"00001822-0000-1000-8000-00805f9b34fb", "Pulse Oximeter"},
    {"00001823-0000-1000-8000-00805f9b34fb", "HTTP Proxy"},
    {"00001824-0000-1000-8000-00805f9b34fb", "Transport Discovery"},
    {"00001825-0000-1000-8000-00805f9b34fb", "Object Transfer"},
    {"00001826-0000-1000-8000-00805f9b34fb", "Fitness Machine"},
    {"00001827-0000-1000-8000-00805f9b34fb", "Mesh Provisioning Service"},
    {"00001828-0000-1000-8000-00805f9b34fb", "Mesh Proxy Service"},
    {"00001829-0000-1000-8000-00805f9b34fb", "Reconnection Configuration"},
    // ... potentially more as the Bluetooth SIG defines new services over time
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Central...");

  // Initialize TFT
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(0);
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0, 0);

  // Initialize BLE
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  if (!pBLEScan) {
    Serial.println("Failed to create BLE scan");
    tft.println("Failed to create BLE scan");
    return;
  }
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
}

void printInfo(const std::string& uuid, const char* type) {
    Serial.printf("\t%s UUID: %s", type, uuid.c_str());
    tft.printf("\t%s UUID: %s", type, uuid.c_str());
    if (sigUUIDs.find(uuid) != sigUUIDs.end()) {
        Serial.printf(" (%s)", sigUUIDs[uuid].c_str());
        tft.printf(" (%s)", sigUUIDs[uuid].c_str());
    }
    Serial.println();
    tft.println();
}

void loop() {
  Serial.println("Scanning...");
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.println("Scanning...");

  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  Serial.print("Devices found: ");
  Serial.println(foundDevices.getCount());
  tft.print("Devices found: ");
  tft.println(foundDevices.getCount());

  for (int i = 0; i < foundDevices.getCount(); i++) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 0);

    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    Serial.printf("Device address: %s, Name: %s\n", device.getAddress().toString().c_str(), device.getName().c_str());
    tft.printf("Device address: %s, Name: %s\n", device.getAddress().toString().c_str(), device.getName().c_str());

    BLEClient* pClient = BLEDevice::createClient();
    if (!pClient) {
      Serial.println("Failed to create client");
      tft.println("Failed to create client");
      continue;
    }

    if (pClient->connect(&device)) {
      Serial.println("Connected to device");
      tft.println("Connected to device");

      std::map<std::string, BLERemoteService*>* services = pClient->getServices();
      if (services) {
        for (auto it = services->begin(); it != services->end(); ++it) {
          BLERemoteService* pService = it->second;
          if (pService) {
            printInfo(pService->getUUID().toString(), "Service");

            std::map<std::string, BLERemoteCharacteristic*>* characteristics = pService->getCharacteristics();
            if (characteristics) {
              for (auto it = characteristics->begin(); it != characteristics->end(); ++it) {
                BLERemoteCharacteristic* pCharacteristic = it->second;
                if (pCharacteristic) {
                  printInfo(pCharacteristic->getUUID().toString(), "Characteristic");

                  if (pCharacteristic->canRead()) {
                    std::string value = pCharacteristic->readValue();
                    if (value.length() > 50) { // Limit the length to avoid overflow
                      value = value.substr(0, 50) + "...";
                    }
                    Serial.printf(" Value: %s", value.c_str());
                    tft.printf(" Value: %s", value.c_str());
                  }

                  if (pCharacteristic->canWrite()) {
                    Serial.print(" [Writable]");
                    tft.print(" [Writable]");
                  }

                  Serial.println();
                  tft.println();
                }
              }
            }
          }
        }
      }

      pClient->disconnect();
      Serial.println("Disconnected from device");
      tft.println("Disconnected from device");
    } else {
      Serial.println("Failed to connect to device");
      tft.println("Failed to connect to device");
    }

    delete pClient;
    delay(3000);  // Add a delay to allow reading the TFT screen before clearing for the next device
  }

  delay(1000);
}
