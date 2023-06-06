struct struct_measure_result {
  int AD_value;
  float AD_voltage;
  float value;
  float everage_10;
};
struct struct_measure_result_hex {
  int MSB_value;
  int LSB_value;
  int MSB_everage_10;
  int LSB_everage_10;
};
struct struct_measure_range {
  float minimum;
  float maximum;
};
struct struct_sensor {
  char unit[10];
  uint8_t input_pin;
  float scale;
  float voltage_at_0_point;
  struct_measure_range measure_range;
};


class class_sensor
{
  private:
    struct_measure_result measure_result;
    struct_sensor data_current_sensor;
    float average_array[10];
    int pointer_next_to_write;

  public:
    void init(struct_sensor got_data_current_sensor) {
      data_current_sensor = got_data_current_sensor;
      pinMode(data_current_sensor.input_pin, INPUT);
    }

    struct_measure_result read(bool init_average) {
      int i;
      measure_result.AD_value = analogRead(data_current_sensor.input_pin);
      measure_result.AD_voltage = ( float) measure_result.AD_value * 5 / 1023;
      measure_result.value = (measure_result.AD_voltage - data_current_sensor.voltage_at_0_point) / data_current_sensor.scale;

      if (init_average == true) {
        for (pointer_next_to_write = 0; pointer_next_to_write < 10; pointer_next_to_write++) {
          average_array[pointer_next_to_write] = measure_result.value;
        }
        pointer_next_to_write = 0;
      }
      else {
        average_array[pointer_next_to_write] = measure_result.value;
        pointer_next_to_write++;
        if (pointer_next_to_write > 9) {
          pointer_next_to_write = 0;
        }
      }
      measure_result.everage_10 = 0;
      for (i = 0; i < 10; i++) {
        measure_result.everage_10 = measure_result.everage_10 + average_array[i];
        average_array[pointer_next_to_write] = measure_result.value;
      }
      measure_result.everage_10 = measure_result.everage_10 / 10;


      return (measure_result);
    }

    void print_result(){
      Serial.print((measure_result.value*1000));
      Serial.print(";");
      Serial.print((measure_result.everage_10*1000));
    }

    void dump_measurement_result() {
      Serial.print("Read value from AD converter: ");
      Serial.print(measure_result.AD_value);

      Serial.print("\tCalculated Voltad from AD converter: ");
      Serial.print(measure_result.AD_voltage, 3);
      Serial.print("V");

      Serial.print("\tresult: ");
      Serial.print(measure_result.value, 3);
      Serial.print(data_current_sensor.unit);

      Serial.print("\teverage (10) result: ");
      Serial.print(measure_result.everage_10, 3);
      Serial.print(data_current_sensor.unit);

      Serial.println("");
    }

      void dump_sensor_data() {
        char pin_readable[5];
        char temp_int_as_char[5];

        Serial.println("-------------------------------------------------");

        Serial.print("Input Pin             : ");
        if (data_current_sensor.input_pin > 13) {
          itoa(data_current_sensor.input_pin - 14, temp_int_as_char, 10);
          strcpy(pin_readable, "A");
          //itoa(data_current_sensor[1].input_pin,pin_readable,10);
          strcat(pin_readable, temp_int_as_char);
          //strcat(pin_readable,"S");
        }

        Serial.println(pin_readable);

        Serial.print("Scale                 : ");
        Serial.print(data_current_sensor.scale, 3);
        Serial.print(" V/");
        Serial.println(data_current_sensor.unit);

        Serial.print("Voltage @ 0 point     : ");
        Serial.print(data_current_sensor.voltage_at_0_point, 3);
        Serial.println(" V");

        Serial.print("Measurement range min : ");
        Serial.print(data_current_sensor.measure_range.minimum, 3);
        Serial.println(data_current_sensor.unit);

        Serial.print("Measurement range max : ");
        Serial.print(data_current_sensor.measure_range.maximum, 3);
        Serial.println(data_current_sensor.unit);

        Serial.print("Unit of sensor        : ");
        Serial.println(data_current_sensor.unit);

        Serial.println("-------------------------------------------------");


      }
    };
