#include <OneWire.h>
#include <DallasTemperature.h>


const int RELAY_Pump = D2;
const int RELAY_Led = D3;
const int RELAY_Heat = D4;


unsigned long min_to_millis(float minutes);
unsigned long hours_to_millis(float hours);
void print_temp_status(float tempC);
void print_module_status(int module_status, char* module_name);
DallasTemperature initSensor(int bus_number);
float getTemperature(DallasTemperature sensors);
int activate_heat(float temp, float min_temp, float max_temp, unsigned long t_heat, unsigned long *last_heat_activation);
int activate_pump(bool pump_state, unsigned long t_pump_on, unsigned long t_pump_off, unsigned long *last_pump_activation);

DallasTemperature temp_water = initSensor(D1);
bool pump_state = 1;

float min_temp = 32.0;
float max_temp = 34.0;

const unsigned long start_time = millis();
unsigned long last_heat_activation, last_led_activation, last_pump_activation = 0;

unsigned long t_heat = min_to_millis(5);
unsigned long t_led_on = hours_to_millis(10);
unsigned long t_led_off = hours_to_millis(14);
unsigned long t_pump_on = min_to_millis(2);
unsigned long t_pump_off = min_to_millis(20);

char p[] = "pompe";
char h[] = "chauffage";

void setup() {
  
  Serial.begin(9600);
  pinMode(RELAY_Pump, OUTPUT);
  pinMode(RELAY_Heat, OUTPUT);
  pinMode(RELAY_Led, OUTPUT);
  digitalWrite(RELAY_Heat, HIGH);
  digitalWrite(RELAY_Pump, HIGH);

}

void loop() {
  // put your main code here, to run repeatedly:
  float temp = getTemperature(temp_water);
  int heat_status = activate_heat(temp, min_temp, max_temp, t_heat, &last_heat_activation);
  int pump_status = activate_pump(pump_state, t_pump_on, t_pump_off, &last_pump_activation);
  print_temp_status(temp);
  print_module_status(heat_status, p);
  print_module_status(pump_status, h);
  
  if (heat_status != -1){
    digitalWrite(RELAY_Heat, heat_status);
  }

  if (pump_status != -1){
    digitalWrite(RELAY_Pump, pump_status);
  }
  delay(10000);

}


DallasTemperature initSensor(int bus_number){

  OneWire oneWire(bus_number);
  DallasTemperature sensors(&oneWire);

  return sensors;

}

float getTemperature(DallasTemperature sensors){
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  return tempC;
}


unsigned long min_to_millis(float minutes){
  return minutes*60*1000;
}

unsigned long hours_to_millis(float hours){
  return hours*60*60*1000;
}

int activate_heat(float temp, float min_temp, float max_temp, unsigned long t_heat, unsigned long *last_heat_activation){
  if (temp < min_temp && millis() - *last_heat_activation > t_heat){
    *last_heat_activation = millis();
    return 1;
  }

  else if (temp > max_temp && millis() - *last_heat_activation > t_heat){
    return 0;
  }
  
  else{
    return -1;
  }
}

int activate_pump(bool pump_state, unsigned long t_pump_on, unsigned long t_pump_off, unsigned long *last_pump_activation){
  if (pump_state == 0 && millis() - *last_pump_activation > t_pump_off){
      last_heat_activation = millis();
      return 1;
  }

  else if (pump_state == 1 && millis() - *last_pump_activation > t_pump_on){
    return 0;
  }
  
  else{
    return -1;
  }
}

void print_temp_status(float tempC){
  Serial.print("La temperature du bassin est de : ");
  Serial.print(tempC);
  Serial.print(" \xC2\xB0");
  Serial.println("C \n");  
}


void print_module_status(int module_status, char* module_name){
  if (module_status == 1){
    Serial.print("La module : ");
    Serial.print((char*)module_name);
    Serial.println(" est ON\n");
  }

  if (module_status == 0){
    Serial.print("La module : ");
    Serial.print((char*)module_name);
    Serial.println(" est OFF\n");
  }

  else{
    Serial.print(".");
  }

}