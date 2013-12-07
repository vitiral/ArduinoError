
#include "debug.h"
#include <Serial.h>
#include <Arduino.h>

const char *DBG_GEN_END_MSG = ")| ";
const char DBG_GEN_ST_MSG[] = {0xA, 0xD,':', ':', 0};
char *DBG_CLEAR_ERROR_MSG = "Cleared Error";
char *DBG_EMPTY_STR = "";

char *errmsg = DBG_EMPTY_STR;
int derr = 0;

void dbg_printinfo(char *file, unsigned int line)
{
  Serial.print(derr); 
  Serial.write(' '); 
  Serial.print(file); 
  Serial.write(':'); 
  Serial.print(line);
}


void DBG_printerrno(){
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
  
  case 20:
    Serial.print(F("TypeErr")); break;
  case 21:
    Serial.print(F("ValueErr")); break;
  case 22:
    Serial.print(F("AssertErr")); break;
  
  case 253:
    Serial.print(F("NoNew")); break;
  case 254:
    Serial.print(DBG_EMPTY_STR); break;
  case 255:
  default:
    Serial.print(F("UnknownErr:")); break;
  }
  Serial.write(')');
  Serial.print(errmsg);
}

void DBG_start_debug(char *file, unsigned int line){
  Serial.print(DBG_GEN_ST_MSG);
  Serial.print(F("DBG: (")); 
  dbg_printinfo(file, line); 
  Serial.print(DBG_GEN_END_MSG);
}

void DBG_start_info(char *file, unsigned int line){
  Serial.print(DBG_GEN_ST_MSG);
  Serial.print(F("INFO: ("));
  dbg_printinfo(file, line);
  Serial.print(DBG_GEN_END_MSG);
}

void DBG_log_err(char *file, unsigned int line){
  Serial.print(DBG_GEN_ST_MSG);
  Serial.print(F("[ERR](")); 
  dbg_printinfo(file, line);
  Serial.print(DBG_GEN_END_MSG); 
  DBG_printerrno();
  if(errno) errno = ERR_NONEW;
  errmsg = DBG_EMPTY_STR;
}

void clrerr(){
  derr = 0; 
  errno = 0; 
  errmsg = DBG_EMPTY_STR;
}

