//////////////////////////////////////////////////////////
////         Messanlage Strom / Spannung              ////
//////////////////////////////////////////////////////////



//Bibliothek für die Vektor Kommunikation

#include <SPI.h>        //Library for using SPI Communication 
#include <mcp2515.h>    //Library for using CAN Communication (https://github.com/autowp/arduino-mcp2515/)
#define DHTPIN 8
#define DHTTYPE DHT11
struct can_frame canMsg;
MCP2515 mcp2515(10);

//////////////////////////////////////////////////////

// StromSensor:

int Strom_eingang = A0;   //Eingangspin für die Strommessung
int Strom_gemessen = 0;         //Wert einfügen und bei 0 anfangen zu zählen
int Nullpunkt = 2500;           //Hälfte der Spannung in mV die vom Arduino kommen

int VpA = 185;                    //für den Sensor mit +/- 5 Amp. gilt dieser Wert
map(VpA, -5, 5, 0, 1023)




double SensorSpannung = 0;      //Die Spannnug die auf dem Sensor vorliegt
double Strom_umgerechnet = 0;      //Wert der umgerechnet wird, final [mA]

////////////////////////////////////////////////////

// SpannungsSensor:

int Spannung_min = 0;              //minimaler Wert den der sensor ermitteln kann
int Spannung_max = 25000;          //maximaler Wert den der Sensor ermitteln kann  in A
const int Spannung_eingang = A1;   //Eingangspin für die Spannungsmessung
int Spannung_gemessen = 0;         //Wert einführen und bei 0 anfangen zu zählen
int Spannung_umgerechnet = 0;      //Wert der mit dem Map-Befehl umgerechnet wird, final [mV]

///////////////////////////////////////////////////


void setup() {
  
  Serial.begin(9600);          // Seriellen Monitor starten

    SPI.begin();                 //beginn der SPI kommunikation
    mcp2515.reset();
    mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz
    mcp2515.setNormalMode();
}



void loop() {


  Spannung_gemessen = analogRead(Spannung_eingang);   //Wert wird ausgeles und abgespeichert
  Spannung_umgerechnet = map(Spannung_gemessen, 0, 1023, Spannung_min, Spannung_max);   //Wert wird umgerechnet um die Spannung in [mV] zu bekommen
  


  Strom_gemessen = analogRead(Strom_eingang);         //Wer wird ausgelesen und abgespeichert
  SensorSpannung = (Strom_gemessen / 1023.0 ) * 5000; //Wird benötigt um auf den Strom umzurechnen
  //Strom_umgerechnet = ((SensorSpannung - Nullpunkt)/VpA);  //Wer wird umgerechnet um den Strom in [mA] zu bekommen
  Strom_umgerechnet=map(Strom_gemessen, -5, 5, 0, 1023)
  

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Serial.print(" Spannung = ");           //sehen was ausgegeben wird       //Kontrolle über den SeriellenMonitor -->  Strg+Umsch+M
Serial.print(Spannung_umgerechnet);     //sehen was ausgegeben wird       //Kontrolle über den SeriellenMonitor
Serial.print("          Strom = ");     //sehen was ausgegeben wird       //Kontrolle über den SeriellenMonitor
Serial.println(Strom_umgerechnet);      //sehen was ausgegeben wird       //Kontrolle über den SeriellenMonitor
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  //Can-Messsage senden:
  
     int Spannung1 = (Spannung_umgerechnet/256);
     int Spannung2 = (Spannung_umgerechnet-(256*Spannung1));

     int Strom1 = (Strom_umgerechnet/256);
     int Strom2 = (Strom_umgerechnet-(256*Strom1));

    canMsg.can_id  = 0x1;             //CAN id as 0x036
    canMsg.can_dlc = 4;               //CAN data length as 4

                        //Aufbau:
                         //0x3412
                         //canMsg.data[0] = 0x12;            //Update humidity value in [0]
                         //canMsg.data[1] = 0x34;            //Update temperature value in [1]


    // Spannung
    canMsg.data[0] = Spannung1;            //Update humidity value in [0]
    canMsg.data[1] = Spannung2;            //Update temperature value in [1]

    //Strom
    canMsg.data[2] = Strom1;            //Rest all with 0
    canMsg.data[3] = Strom2;

    mcp2515.sendMessage(&canMsg);     //Sends the CAN message
  
  delay(300);             //kurzer delay, wird aber noch dorch intrerrupt ersetzt

}
