/*-----------------------------------------------------
parametr1 - вкл/выкл светодиод
parametr2 - значение потенциометра в пределах [0; 255]
Формат строки:
DID=1;CMD=2;PRM=2;DAT=123
Для включения светодиода:
DID=1;CMD=1;PRM=1;DAT=1
Для выключения светодиода:
DID=1;CMD=1;PRM=1;DAT=0
------------------------------------------------------*/

#include <RFM69.h>
#include <SPI.h>

#define DID           20     // device ID
#define DEST_ID       21     // destination ID
#define NETWORKID     100 
 
#define IS_RFM69HW 
#define ACK_TIME       50                  // max msec for ACK wait
#define MAX_BUFF       32                  // buffer length
#define LED            9                   
#define SERIAL_BAUD    115200
#define FREQUENCY      RF69_433MHZ   
#define ENCRYPT_KEY    "my-EncryptionKey"

RFM69 radio;                  // global radio instance
bool promiscuousMode = false; // set 'true' to sniff all packets on the same network
bool requestACK=false;

char DID_in[10];
char CMD_in[10];
char PRM_in[10];
char DAT_in[100];
int led_pin = A0;

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(10);
  radio.initialize(FREQUENCY, DID, NETWORKID);
  radio.encrypt(ENCRYPT_KEY);
  radio.promiscuous(promiscuousMode);
#ifdef IS_RFM69HW
  radio.setHighPower(); //uncomment #define ONLY if radio is of type: RFM69HW or RFM69HCW 
#endif

  pinMode(led_pin, OUTPUT);
  
  Serial.print("Light module start. Network ID: ");Serial.print(NETWORKID);
  Serial.print(". Device ID: ");
  Serial.println(DID);
  delay(50);  
}

int c_count = 0;

void loop() {
  if (Serial.available() > 0)
  {
    delay(10);
    c_count = Serial.available();
    byte i = 0;
    char buff[c_count];
    if (c_count <= MAX_BUFF)
    {
      while(Serial.available()) 
        buff[i++] = Serial.read();
      buff[i++] = '\0';
    }     
     radio.send(DEST_ID, buff, (byte)sizeof(buff), requestACK);
     Blink(LED,3);
     Serial.print("[I]< ");
     Serial.println(buff);
  }
  
  if (radio.receiveDone()) {
    char radio_buff[radio.DATALEN];
    byte i = 0;
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("]> ");
    for (i = 0; i < radio.DATALEN; i++)
      radio_buff[i] = (char)radio.DATA[i];
    radio_buff[i++] = '\0';
    Serial.print(radio_buff);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.println("]");    
    Blink(LED,3);
     
    sscanf(radio_buff, "%*c%*c%*c%*c%[^';'];%*c%*c%*c%*c%[^';'];%*c%*c%*c%*c%[^';'];%*c%*c%*c%*c%s",
      &DID_in, &CMD_in, &PRM_in, &DAT_in);
   
    if (((String)DAT_in == "1") && ((String)CMD_in == "1") && ((String)PRM_in == "1")) digitalWrite(led_pin, HIGH);
    if (((String)DAT_in == "0") && ((String)CMD_in == "1") && ((String)PRM_in == "1")) digitalWrite(led_pin, LOW);
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

