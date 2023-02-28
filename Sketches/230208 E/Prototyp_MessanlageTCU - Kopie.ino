
#include <SPI.h>//Library for using SPI Communication 
#include <mcp2515.h>//Library for using CAN Communication (https://github.com/autowp/arduino-mcp2515/)
#define DHTPIN 8
#define DHTTYPE DHT11
struct can_frame canMsg;
MCP2515 mcp2515(10);

const int ANALOG_IN_PIN = A1;                       // Voltage
const int currentPin = A0;                          // Ampere


float adc_voltage = 0.0;                            // Voltage
float in_voltage = 0.0;                             // Voltage             
float R1 = 30000.0;                                 // Voltage
float R2 = 7500.0;                                  // Voltage
float ref_voltage = 5.0;                            // Voltage
int adc_value = 0;                                  // Voltage

int sensitivity = 66;                               // Ampere
int adcValue= 0;                                    // Ampere
int offsetVoltage = 2500;                           // Ampere
double adcVoltage = 0;                              // Ampere
double currentValue = 0;                            // Ampere

////////////////////////////////
int Volt1 = 0;
int Volt2 = 0;

int Amp1 = 0;
int Amp2 = 0;
///////////////////////////////
 
void setup()
{
   while (!Serial);
  Serial.begin(9600);       // Seriellen Monitor starten
  Serial.println("Strom und Spannung");           // Überschrift der Messung
  SPI.begin();               //Begins SPI communication
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();
                              
   
}

 
void loop(){
   
   adc_value = analogRead(ANALOG_IN_PIN);          // Analogen Pin auslesen (Volt)
   adc_voltage  = (adc_value * ref_voltage) / 1024.0;   // ADC bestimmen -> Analog to Digital Conversion
   in_voltage = adc_voltage / (R2/(R1+R2)) ;       // Volt berechnen
   in_voltage = int(in_voltage*1000);                  // Volt in MILIVolt umrechnen

   Volt1 = int(in_voltage/256);
   Volt2 = (in_voltage-(256*Volt1));

   
  Serial.print(" Voltage = ");               // Volt ausgeben
  Serial.print(in_voltage, 0);


  
  adcValue = analogRead(currentPin);            // Analogen Pin auslesen  (Amperé)
  adcVoltage = (adcValue / 1024.0) * 5000;      // ADC bestimmen -> Analog to Digital Conversion
  currentValue = ((adcVoltage - offsetVoltage) / sensitivity);   // Amperé berechnen
  currentValue = int( currentValue*1000);          // Amperé in MILIAmperé umrechnen


   Amp1 = int(currentValue/256);
   Amp2 = (currentValue-(256*Amp1));

   
  Serial.print("    Amperé = ");
  Serial.println(currentValue,0);
 
 
  canMsg.can_id  = 0x1;             //CAN id as 0x036
  canMsg.can_dlc = 4;               //CAN data length as 8
  //Spannung
  //0x3412
  //canMsg.data[0] = 0x12;            //Update humidity value in [0]
  //canMsg.data[1] = 0x34;            //Update temperature value in [1]

  canMsg.data[0] = Volt1;            //Update humidity value in [0]
  canMsg.data[1] = Volt2;            //Update temperature value in [1]
  //Strom
  canMsg.data[2] = Amp1;            //Rest all with 0
  canMsg.data[3] = Amp2;
  
  mcp2515.sendMessage(&canMsg);     //Sends the CAN message

  delay(100);                                     // Short delay
}
