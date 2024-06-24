#include <SPI.h>
#include <MFRC522.h>


#define RST_PIN 0  // TODO: replace with actual reference ports
#define SS_PIN 15


enum Operation { READ,WRITE,COPY };


MFRC522 rfc(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte cardContents[1024];
byte testData[12] = {"Hello-world"};
Operation operation = Operation::READ;


void setup() {
  Serial.begin(115000);
  SPI.begin();
  rfc.PCD_Init();
  delay(4);
}

void loop() {
  switch (operation) {
    case READ:
      readCard();
      break;
    case WRITE:

      break;
    case COPY:

      break;
    default:
      break;
  }
}

void readCard() {
  if ( !rfc.PICC_IsNewCardPresent()) {
    return;
  }

  if ( !rfc.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println("Card UID:");
  printArray( rfc.uid.uidByte, rfc.uid.size);
  Serial.println();
  Serial.println("PICC type: ");
  MFRC522::PICC_Type piccType = rfc.PICC_GetType(rfc.uid.sak);
  Serial.println( rfc.PICC_GetTypeName(piccType));

  if ( piccType != MFRC522::PICC_TYPE_MIFARE_1K
    && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }

  switch(piccType){
    case MFRC522::PICC_TYPE_MIFARE_1K:
      for (byte i = 0; i < 6; i++){
        key.keyByte[i] = 0xFF;
      }

      // writeBlock(1, testData);
      readSegment_1k();
      break;
    // case MFRC522::PICC_TYPE_MIFARE_4K: // TODO: 4k support
    default:
      break;
  }

  rfc.PICC_HaltA();
  rfc.PCD_StopCrypto1();


}

void readSegment_1k(){
  byte segCount = 16;

  for(byte i = 0; i < segCount; i++){
    Serial.println(readBlock(i, &(cardContents[(i-1) * 16 * 4])));
  }

  Serial.println("------------------Output array----------------");
  printArray(&cardContents[0], sizeof(cardContents));
}

String readBlock(int blockNumber, byte arrayAddress[]){

  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;
  
  MFRC522::StatusCode status = rfc.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfc.uid));
  if (status != MFRC522::STATUS_OK) {
    return ( 
        String("Authentication failed : ") + rfc.GetStatusCodeName(status)
      );
  }

  byte buffersize = 18;
  status = rfc.MIFARE_Read(blockNumber, arrayAddress, &buffersize);
  if (status != MFRC522::STATUS_OK) {
    return (
      String("Read error") + rfc.GetStatusCodeName(status)
    );
  }

  return (
    String("Block read successfully: ") + blockNumber
  );
}

int writeBlock(int blockNumber, byte arrayAddress[])
{
  int largestModulo4Number = blockNumber / 4 * 4;
  int trailerBlock = largestModulo4Number + 3;
  if (blockNumber > 2 && (blockNumber + 1) % 4 == 0) {
    Serial.print(blockNumber);
    Serial.println(" is a trailer block: Error");
    return 2;
  }

  MFRC522::StatusCode status = rfc.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfc.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("Authentication failed: ");
    Serial.println(rfc.GetStatusCodeName(status));
    return 3;
  }

  status = rfc.MIFARE_Write(blockNumber, arrayAddress, 16);

  if (status != MFRC522::STATUS_OK) {
    Serial.print("Data write failed: ");
    Serial.println(rfc.GetStatusCodeName(status));
    return 4;
  }
  Serial.print("Data written to block ");
  Serial.println(blockNumber);
  return 0;
}

void printArray(byte *buffer, short bufferSize) {
  for (short i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }

  Serial.println();
  Serial.println(String((char*) buffer));
}
