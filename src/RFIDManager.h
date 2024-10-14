#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <Arduino.h>
#include <MFRC522.h>

class RFIDManager {
public:
    RFIDManager(int ssPin, int rstPin);
    void setup();
    bool checkCard();
    String getLastTagId() const;

private:
    MFRC522 mfrc522;
    String lastTagId;

    bool isAuthorizedTag(const String& tagId);
};

#endif // RFIDMANAGER_H
