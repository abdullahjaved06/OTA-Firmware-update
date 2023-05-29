// Read the response for URL:
  Available_firmware_version = http.getString();
  Serial.println("Got HTTP response:");
  Available_firmware_version.remove(0, 1);                                        //remove 1 character starting at index 0
  Available_firmware_version.remove(Available_firmware_version.length() - 1, 1);  ////remove 1 character end at index n-1
Serial.println("checking for OTA update";)

  if (Available_firmware_version == Current_Version) {
    Serial.println("no Updates available");
    ESP.restart();
  }

  else if (Available_firmware_version != Current_Version) {

    // Start pulling down the firmware binary.
    Serial.print("current firmware version : ");
    Serial.println(Current_Version);
    Serial.print("Available_firmware_version : ");
    Serial.println(Available_firmware_version);
    delay(2000);
    http.begin(FIRMWARE_URL);
    int httpCode = http.GET();
    if (httpCode <= 0) {
      Serial.printf("HTTP failed, error: %s\n",
                    http.errorToString(httpCode).c_str());
      return;
    }
    // Check that we have enough space for the new binary.
    int contentLen = http.getSize();
    Serial.printf("Content-Length: %d\n", contentLen);
    bool canBegin = Update.begin(contentLen);
    if (!canBegin) {
      Serial.println("Not enough space to begin OTA");
      return;
    }
    // Write the HTTP stream to the Update library.
    WiFiClient *client = http.getStreamPtr();
    size_t written = Update.writeStream(*client);
    Serial.printf("OTA: %d/%d bytes written.\n", written, contentLen);
    if (written != contentLen) {
      Serial.println("Wrote partial binary. Giving up.");
      return;
    }
    if (!Update.end()) {
      Serial.println("Error from Update.end(): " + String(Update.getError()));
      return;
    }
    if (Update.isFinished()) {
      Serial.println("Update successfully completed. Rebooting.");
      // This line is specific to the ESP32 platform:
      ESP.restart();
    } else {
      Serial.println("Error from Update.isFinished(): " + String(Update.getError()));
      return;
    }
  }