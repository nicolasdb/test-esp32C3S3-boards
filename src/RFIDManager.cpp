#include "RFIDManager.h"
#include <SPIFFS.h>

RFIDManager::RFIDManager(int ssPin, int rstPin) : mfrc522(ssPin, rstPin) {}

void RFIDManager::setup() {
    SPI.begin();
    mfrc522.PCD_Init();
    Serial.println("RFID reader initialized");
}

bool RFIDManager::checkCard() {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        lastTagId = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            lastTagId += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
            lastTagId += String(mfrc522.uid.uidByte[i], HEX);
        }
        lastTagId.toUpperCase();
        
        Serial.println("RFID Tag detected: " + lastTagId);
        
        bool authorized = isAuthorizedTag(lastTagId);
        if (authorized) {
            Serial.println("Authorized tag. Opening door.");
        } else {
            Serial.println("Unauthorized tag. Access denied.");
        }
        
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        
        return authorized;
    }
    return false;
}

String RFIDManager::getLastTagId() const {
    return lastTagId;
}

bool RFIDManager::isAuthorizedTag(const String& tagId) {
    File file = SPIFFS.open("/authorized_tags.csv", "r");
    if (!file) {
        Serial.println("Failed to open authorized_tags.csv");
        return false;
    }
    
    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        if (line == tagId) {
            file.close();
            return true;
        }
    }
    
    file.close();
    return false;
}
