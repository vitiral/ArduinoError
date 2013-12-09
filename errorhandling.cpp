
#include "errorhandling.h"
#include <Serial.h>
#include <Arduino.h>

const char *EH_GEN_END_MSG = ")| ";
//const char EH_GEN_ST_MSG[] = {0xA, 0xD,':', ':', 0};
const char EH_GEN_ST_MSG[] = ":: ";
char *EH_EMPTY_STR = "";

char *errmsg = EH_EMPTY_STR;
int derr = 0;
int errno = 0;

void EH_printinfo(char *file, unsigned int line)
{
  Serial.print(derr); 
  Serial.write(' '); 
  Serial.print(file); 
  Serial.write(':'); 
  Serial.print(line);
}


void EH_printerrno(){
  Serial.write('(');
  Serial.print(errno);
  Serial.write(':');
  
  switch(errno){
  case 0:
    Serial.print(F("NoErr")); break;
  case 1:
    Serial.print(F("BaseErr")); break;
  case 2:
    Serial.print(F("TimeoutErr")); break;
  case 3:
    Serial.print(F("SerialErr")); break;
  case 4:
    Serial.print(F("SpiErr")); break;
  case 5:
    Serial.print(F("I2cErr")); break;
  case 6:
    Serial.print(F("ComErr")); break;
  case 7:
    Serial.print(F("ConfigErr")); break;
  case 8:
    Serial.print(F("PinErr")); break;
  case 9:
    Serial.print(F("InputErr")); break;
  
  case 50:
    Serial.print(F("TypeErr")); break;
  case 51:
    Serial.print(F("ValueErr")); break;
  case 52:
    Serial.print(F("AssertErr")); break;
  
  case 252:;
    Serial.print(F("Cleared Error")); break;
  case 253:
    Serial.print(F("NoNew")); break;
  case 254:
    Serial.print(EH_EMPTY_STR); break;
  case 255:
  default:
    Serial.print(F("UnknownErr:")); break;
  }
  Serial.write(')');
  Serial.print(errmsg);
}

void EH_start_debug(char *file, unsigned int line){
  Serial.print(EH_GEN_ST_MSG);
  Serial.print(F("DBG: (")); 
  EH_printinfo(file, line); 
  Serial.print(EH_GEN_END_MSG);
}

void EH_start_info(char *file, unsigned int line){
  Serial.print(EH_GEN_ST_MSG);
  Serial.print(F("INFO: ("));
  EH_printinfo(file, line);
  Serial.print(EH_GEN_END_MSG);
}

void EH_log_err(char *file, unsigned int line){
  Serial.print(EH_GEN_ST_MSG);
  Serial.print(F("[ERR](")); 
  EH_printinfo(file, line);
  Serial.print(EH_GEN_END_MSG); 
  EH_printerrno();
  if(errno) errno = ERR_NONEW;
  errmsg = EH_EMPTY_STR;
}

void clrerr(){
  derr = 0; 
  errno = 0; 
  errmsg = EH_EMPTY_STR;
}

void seterr(int error){
  derr = error;
  errno = error;
  errmsg = EH_EMPTY_STR;
}

void EH_test(){
  Serial.println("Doing dbg test");
}

