#define DEBUG
#include "errorhandling.h"

short int isint(char c){
  if(('0' <= c) and (c <= '9')) return true;
  else return false;
}

int char_to_int(char c){
  return c - '0';
}

int get_user_int(){
  while(!Serial.available());
  char c = Serial.read();
  debug(isint(c)); // Doing a debug check to check value.
  assert_raisem(isint(c), ERR_TYPE, c, HEX); //note: raisem raises a message too.
  //assert_raise(isint(c), ERR_TYPE);
  return char_to_int(c);
error:
  return -1;
}

// Top Level Function
void print_user_int(){
  int user_int = get_user_int(); // expects to get an integer from the user
  noerr_log();  // if there was an error, log it and go to error:
  Serial.println(user_int);
  return;
error:
  clrerr_log(); //clears error flags and logs that it did so.
  return;
}

void setup(){
  Serial.begin(9600);
  log_info(String("Set up at baud: ") + String(9600));
}

void loop(){
  Serial.println("Input Int:");
  print_user_int();
}


