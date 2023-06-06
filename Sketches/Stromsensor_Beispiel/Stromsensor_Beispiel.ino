//test scetch for current sensor
#include "sensor.h"
bool debug = false;

struct_sensor data_current_sensor;
class_sensor current_sensor;

void setup() {
  Serial.begin(9600);          // Seriellen Monitor starten

  //current sensor:
  data_current_sensor.input_pin = A0;
  data_current_sensor.scale = 0.185;
  data_current_sensor.voltage_at_0_point = 2.5;
  data_current_sensor.measure_range.minimum = -5;
  data_current_sensor.measure_range.maximum = 5;
  strcpy(data_current_sensor.unit, "A");
  current_sensor.init(data_current_sensor);

  struct_measure_result current_sensor_result;

  if(debug == true){
    current_sensor.dump_sensor_data();
  }
  current_sensor_result=current_sensor.read(true);
  Serial.println("current;everage current");
}

void loop() {
  struct_measure_result current_sensor_result;
  current_sensor_result=current_sensor.read(false);
  if(debug==true){
  current_sensor.dump_measurement_result();
  }
  current_sensor.print_result();
  Serial.println("");
  delay(1000);
}
