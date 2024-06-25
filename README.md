# RFID utility tool

Tool based on ESP8266 controller and MFRC522 module to read and write memory of  RFID tags. 

Currently, supports **Mifare Classic** 1k tags, more tags support probably would be added in future.

### What's new

- Added methtods to read and write separate blocks (mostly from MFRC522 repo guides)
- Created method to read whole tag memory and display as string
- Currently works only with factory key

### Future plans
- Write method to access whole card memory
- Non-factory keys support
- Changing key capabilities
- Web interface for ESP8266
- 🔍️ RFID UID alteration