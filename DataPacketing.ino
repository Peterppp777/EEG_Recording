#include <LSM6DS3.h>
#include <Arduino.h>
#include <C:\Users\jiesh\Documents\Arduino\libraries\CircularBuffer\CircularBuffer.h> // specific path for circular buffer library
#include <NRF52_MBED_TimerInterrupt.h> // timer interrupt library
#include <Arduino_CRC32.h>
#include "Wire.h"

// Timer
NRF52_MBED_Timer ITimer0(0);
#define TIMER_INTERVAL_MS 1000 //in microsecs

// IMU
LSM6DS3 IMU(I2C_MODE, 0x6A);

// EEG data structure
struct EEGReading {
  unsigned short adcValue;    
};

EEGReading createReading(int adcValue) {
  EEGReading reading;
  reading.adcValue = adcValue;
  return reading;
}

struct IMUReading {
  float accelerometer[3]; // X, Y, Z values for the accelerometer
  float gyroscope[3];     // X, Y, Z values for the gyroscope
};


CircularBuffer<EEGReading, 1000> eegDataBuffer;
CircularBuffer<IMUReading, 100> imuDataBuffer;

// EEG packet structure
struct EEGPacket {
  unsigned short sequenceNumber = 0;
  EEGReading readings[5] = {};  
};

// Global variables
unsigned int currentSampleIndex = 0;
uint16_t eegSequenceNumber = 0;
unsigned int packetCounter = 0;
volatile bool analogFlag;
EEGPacket currentPacket;

volatile bool imuFlag = false;
unsigned long lastIMUTime = 0;
const unsigned long imuInterval = 1000; // 1 second in milliseconds


// Sent packets buffer
CircularBuffer<EEGPacket, 100> sentPacketsBuffer;


// Function declarations
void acquireEEGData();
void acquireIMUData();
void processNewEEGReading(EEGReading reading);
void sendPacket(EEGPacket& packet);
void sendIMU();



void TimerHandler(void) {
  analogFlag = true;
}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  while (!Serial);
  Serial.println("Initializing...");
  if (IMU.begin() != 0) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  if(ITimer0.attachInterruptInterval(TIMER_INTERVAL_MS * 2, TimerHandler)){
    Serial.println("Timer setup successful");
  } else {
    Serial.println("Timer setup failed");
    while (1); // This will block everything, consider handling the error differently // Timer setup failed
  }
}

void loop() {
  // Serial.println("Loop start");
  if(analogFlag == true){
    acquireData();
    analogFlag = false;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastIMUTime >= imuInterval) {
    acquireIMUData();
    lastIMUTime = currentTime; // Update the last IMU acquisition time
    imuFlag = true; // Set the flag to send IMU data
  }

  while (!eegDataBuffer.isEmpty()) {
    EEGReading reading = eegDataBuffer.pop();
    processNewEEGReading(reading);
  }

  if (imuFlag) {
    sendIMU();
    imuFlag = false; // Reset the flag after sending the packet
  }
  // Serial.println("Loop end");
}

void acquireData(void){
  int adcValue = analogRead(A0);
  EEGReading reading = createReading(adcValue);
  eegDataBuffer.push(reading);
}

void acquireIMUData(void){
  IMUReading imuReading;

  // Hypothetical functions to read data from your IMU sensor
  // Replace these with actual functions from your IMU library
  imuReading.accelerometer[0] = IMU.readFloatAccelX();
  imuReading.accelerometer[1] = IMU.readFloatAccelY();
  imuReading.accelerometer[2] = IMU.readFloatAccelZ();
  imuReading.gyroscope[0] = IMU.readFloatGyroX();
  imuReading.gyroscope[1] = IMU.readFloatGyroY();
  imuReading.gyroscope[2] = IMU.readFloatGyroZ();

  imuDataBuffer.push(imuReading);
}

void processNewEEGReading(EEGReading reading) {
  currentPacket.readings[currentSampleIndex++] = reading;
  if (currentSampleIndex >= 5) {
    sendPacket(currentPacket);
    currentSampleIndex = 0;
    currentPacket.sequenceNumber = eegSequenceNumber++;
  }
}


// void processNewEEGReading(EEGReading reading) {
//   currentPacket.readings[currentSampleIndex] = reading;

//   // Print the adcValue of the current reading
//   Serial.print("Reading ");
//   Serial.print(currentSampleIndex);
//   Serial.print(": ");
//   Serial.println(reading.adcValue);

//   currentSampleIndex++;
//   if (currentSampleIndex >= 5) {
//     sendPacket(currentPacket);
//     currentSampleIndex = 0;
//     currentPacket.sequenceNumber = eegSequenceNumber++;
//   }
// }

void sendPacket(EEGPacket& packet) {
    const size_t sequenceNumberSize = sizeof(packet.sequenceNumber);
    const size_t readingsSize = 10; // 5 readings * 2 bytes each = 10 bytes
    const size_t packetSize = sequenceNumberSize + readingsSize;

    const size_t headerSize = 3; // 1 byte for header, 2 bytes for packet length
    const size_t footerSize = 1;
    const size_t crcSize = 4; // CRC32 size (4 bytes)
    const size_t bufferSize = packetSize + headerSize + crcSize + footerSize;
    byte buffer[bufferSize];

    // Add header and packet length
    buffer[0] = 0xAA;
    uint16_t length = packetSize + crcSize; // Include CRC size in packet length
    buffer[1] = (byte)(length & 0xFF);
    buffer[2] = (byte)((length >> 8) & 0xFF);

    // Copy packet data
    memcpy(buffer + headerSize, &packet.sequenceNumber, sequenceNumberSize);

    // Pack the readings into the buffer
    for (int i = 0; i < 5; i++) {
        unsigned int reading = packet.readings[i].adcValue;
        buffer[headerSize + sequenceNumberSize + i * 2] = (byte)(reading & 0xFF); // Lower 8 bits
        buffer[headerSize + sequenceNumberSize + i * 2 + 1] = (byte)((reading >> 8) & 0x0F); // Upper 4 bits
    }

    // Calculate CRC32
    Arduino_CRC32 crc32;
    uint32_t crcValue = crc32.calc(buffer + headerSize, packetSize);

    // Place CRC32 value in the buffer
    buffer[bufferSize - 5] = (byte)(crcValue & 0xFF);
    buffer[bufferSize - 4] = (byte)((crcValue >> 8) & 0xFF);
    buffer[bufferSize - 3] = (byte)((crcValue >> 16) & 0xFF);
    buffer[bufferSize - 2] = (byte)((crcValue >> 24) & 0xFF);

    // Add footer
    buffer[bufferSize - 1] = 0x55;

    // Send the entire buffer
    Serial.write(buffer, bufferSize);

    // Store the packet in the sent buffer
    sentPacketsBuffer.push(packet);
}

void sendIMU(){
  if (imuDataBuffer.isEmpty()) {
    return;
  }

  // Form the packet (this is highly dependent on your specific packet structure and protocol)
    IMUReading reading = imuDataBuffer.pop();

    const size_t readingsSize = 6 * sizeof(float); // 3 accelerometer readings + 3 gyroscope readings, each 4 bytes
    const size_t headerSize = 3; // 1 byte for header, 2 bytes for packet length
    const size_t footerSize = 1;
    const size_t crcSize = 4; // CRC32 size (4 bytes)
    const size_t packetSize = readingsSize;
    const size_t bufferSize = packetSize + headerSize + crcSize + footerSize;
    byte buffer[bufferSize];

    // Add header and packet length
    buffer[0] = 0xEE; // Example header byte
    uint16_t length = packetSize + crcSize; // Include CRC size in packet length
    buffer[1] = (byte)(length & 0xFF);
    buffer[2] = (byte)((length >> 8) & 0xFF);

    // Copy IMU data
    memcpy(buffer + headerSize, &reading.accelerometer, sizeof(reading.accelerometer));
    memcpy(buffer + headerSize + sizeof(reading.accelerometer), &reading.gyroscope, sizeof(reading.gyroscope));

    // Calculate CRC32
    Arduino_CRC32 crc32;
    uint32_t crcValue = crc32.calc(buffer + headerSize, packetSize);

    // Place CRC32 value in the buffer
    buffer[bufferSize - 5] = (byte)(crcValue & 0xFF);
    buffer[bufferSize - 4] = (byte)((crcValue >> 8) & 0xFF);
    buffer[bufferSize - 3] = (byte)((crcValue >> 16) & 0xFF);
    buffer[bufferSize - 2] = (byte)((crcValue >> 24) & 0xFF);

    // Add footer
    buffer[bufferSize - 1] = 0x99; // Example footer byte

    // Send the entire buffer
    Serial.write(buffer, bufferSize);
}