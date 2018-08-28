#include "structures.h"

short bat = 200;
void command (void* data) {
  if (Serial.available() >= 2) {
    int size1 = Serial.available();
    char com = Serial.read();
    int i;
    for (int i = 0; i< size1 - 1; i++) {
      Serial.read(); 
    }
    if (command_enable || com == 'i') {
      if (com == 'i') {
        command_enable = 1;
        Serial.println("Command enabled");
      } else if (com == 's') {
        cur = TOP;
        Serial.println("System started");
      } else if (com == 'p') {
        cur = NONE;
        Serial.println("System stoped");
      } else if (com == 'd') {
        dis_enable = !dis_enable;
        if (!dis_enable) {
          saved = cur;
          cur = NODIS;
          tft.fillScreen(BLACK);
        } else {
          cur = saved;
        }
        Serial.println("Display changed");
      } else if (com == 'm') {
        dataUp = 1;
      } else if (com == 'w') {
        warnUp = 1;
      } else {
        error_message = 1;
      }
    } else {
      error_message = 1;
    }
  }
}

void remoteCom(void* data_in) {
  RemoteComData* data = (RemoteComData*) data_in;
  if (dataUp) {
    // update dataUp
    Serial.println("Data updated");
    *(data->temp_d) = temp_f;
    *(data->sys_d) = sys_f;
    *(data->dias_d) = dis_f;
    *(data->pulse_d) = pul_f;
    *(data->resp_d) = res_f;
    *(data->ekg_d) = ekg_f;
    bat = batteryState;
    dataUp = 0;
  }

  if (warnUp) {
    Serial.println("warning updated");
    *(data->tempW) = tempOutOfRange;
    *(data->sysW) = sysOutOfRange;
    *(data->disW) = diasOutOfRange;
    *(data->pulsW) = pulseOutOfRange;
    *(data->resW) = sysOutOfRange;
    warnUp = 0;
  }
  if (!warnUp && !dataUp) {
    *(data->temp_d) = temp_f;
    *(data->sys_d) = sys_f;
    *(data->dias_d) = dis_f;
    *(data->pulse_d) = pul_f;
    *(data->resp_d) = res_f;
    *(data->ekg_d) = ekg_f;
    *(data->tempW) = tempOutOfRange;
    *(data->sysW) = sysOutOfRange;
    *(data->disW) = diasOutOfRange;
    *(data->pulsW) = pulseOutOfRange;
    *(data->resW) = sysOutOfRange;
    bat = batteryState;

  }
}

unsigned long prevRemote = 9;; 

void remoteDis(void* data_in) {
  RemoteDisData* data = (RemoteDisData*) data_in;
  unsigned long now = timer % 10;

  // all
  if (prevRemote == (unsigned long)9 && now == (unsigned long) 0) {
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");     // cursor to home command
    Serial.println("Health Helper Monitor System");
    Serial.println("Patient name : Jack       Doctor name: Mary");
    Serial.print("Temperature         ");Serial.print(*(data->temp_d));Serial.println(" C");
    Serial.print("Systolic pressure   ");Serial.print(*(data->sys_d));Serial.println(" mmHg");
    Serial.print("Distolic pressure   ");Serial.print(*(data->dias_d));Serial.println(" mmHg");
    Serial.print("Pulse Rate          ");Serial.print(*(data->pulse_d));Serial.println(" Bps");
    Serial.print("Respiration Rate    ");Serial.print(*(data->resp_d));Serial.println(" Bps");
    Serial.print("EKG                 ");Serial.print(*(data->ekg_d));Serial.println(" Hz");
    Serial.print("Battery             ");Serial.print(bat);Serial.println(" Percent");
    
  } else if (prevRemote == (unsigned long) 4 && now == (unsigned long) 5) {
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");     // cursor to home command
    Serial.println("Health Helper Monitor System");
    Serial.println("Patient name : Jack       Doctor name: Mary");
    Serial.print("Temperature         ");
    if (!*(data->tempW)) {
      Serial.print(*(data->temp_d));
      Serial.print(" C");
    } 
    Serial.println();
    
    Serial.print("Systolic pressure   ");
    if (!*(data->sysW)) {
      Serial.print(*(data->sys_d));
      Serial.print(" mmHg");
    } 
    Serial.println();
    
    Serial.print("Distolic pressure   ");
    if (!*(data->disW)) {  
      Serial.print(*(data->dias_d));
      Serial.print(" mmHg");
    } 
    Serial.println();
    
    Serial.print("Pulse Rate          ");
    if (!*(data->pulsW)) {    
      Serial.print(*(data->pulse_d));
      Serial.print(" Bps");
    }
    Serial.println();
   
    Serial.print("Respiration Rate    ");
    if (!*(data->resW)) {      
      Serial.print(*(data->resp_d));
      Serial.print(" Bps");
    }
    Serial.println();
    
    Serial.print("EKG                 ");
    if (!*(data->ekgW)) {  
      Serial.print(*(data->ekg_d));
      Serial.print(" Hz");
    }
    Serial.println();
    Serial.print("Battery             ");Serial.print(bat);Serial.println(" Percent");
  }
  if (error_message) {
    Serial.println("E invalid command");
    error_message = 0;
  }
  prevRemote = timer;

}



