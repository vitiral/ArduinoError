#define DEBUG

#include <SoftwareSerial.h>
#include <errorhandling.h>
#include <ByteBuffer.h>

SoftwareSerial SoftSerial(10, 1); // RX, TX -- 10 should be disconnected (does not receieve)
ByteBuffer failbuffer;

void setup(){
  failbuffer.init(100);
  //setup_std();
  setup_soft();
  log_info("setup");
  debug("Testing Debug");
}

void setup_std(){
  Serial.begin(57600);
  Serial.println("Started Std Serial");
}

void setup_soft(){
  SoftSerial.begin(57600);
  SoftSerial.println("Started Soft Serial");
  EH_config_soft(SoftSerial);
}

void do_failure(){
  char *errmsg = "Doing Failure";
  assert(false);
error:  
  return;
}

void do_failure2(){
  seterr(ERR_VALUE);
  if(true) iferr_log_return();
  clrerr(); // function after this should be expecting derr to exist
}

#define T1_TESTS 21
unsigned short test1(unsigned short tp){
  short tofail = false;
  short tobreak = true;
  switch(tp){
  case 0:
    tofail = true;
    assert(false);
    break;
  case 1:
    assert(true);
    break;
  case 2:
    tofail = true;
    raise(ERR_VALUE);
    break;
  case 3:
    tofail = true;
    raise(ERR_VALUE, 42, HEX);
    break;
  case 4:
    tofail = true;
    assert_raise(false, ERR_VALUE);
    break;
  case 5:
    assert_raise(true, ERR_VALUE);
    break;
  case 6:
    tofail = true;
    seterr(ERR_VALUE);
    iferr_catch();
    break;
  case 7:
    tofail = true;
    seterr(ERR_VALUE);
    iferr_log_catch();
    break;
  case 8:
    seterr(ERR_VALUE);
    clrerr();
    iferr_catch();
    break;
  case 9:
    seterr(ERR_VALUE);
    log_info("Clearing error and logging");
    clrerr_log();
    iferr_catch();
    break;
  case 10:
    tobreak = false;
    assert_return(false, 0);
    break;
  case 11:
    tobreak = true;
    assert_return(true, 1);
    break;
  case 12:
    tobreak = false;
    raise_return(ERR_VALUE, 0);
    break;
  case 13:
    tobreak = false;
    raisem_return(ERR_VALUE, 42, 0);
    break;
  case 14:
    tobreak = false;
    assert_raise_return(false, ERR_VALUE, 0);
    break;
  case 15:
    tobreak = true;
    assert_raise_return(true, ERR_VALUE, 1);
    break;
  case 16:
    tobreak = false;
    seterr(ERR_VALUE);
    //iferr_return 0;
    iferr_return(0);
    break;
  case 17:
    tobreak = false;
    seterr(ERR_VALUE);
    iferr_log_return(0);
    //iferr_log_return 0;
    break;
  case 18:
    tobreak = true;
    seterr(ERR_VALUE);
    clrerr();
    iferr_return(1);
    break;
  case 19:
    tobreak = true;
    seterr(ERR_VALUE);
    log_info("Clearing error and logging");
    clrerr_log();
    iferr_return(1);
    break;
  case 20:
    tobreak = true;
    do_failure2();
    assert(derr);
    break;
  case 21:
    tobreak = true;
    // testing syntax
    seterr(ERR_VALUE);
    if(false) raise(ERR_VALUE);
    if(false) assert(false);
    if(false) assert_raise(false, ERR_VALUE);
    if(false) iferr_return(1);
    if(false) raise_return(ERR_VALUE, 1);
    if(false) raisem_return(ERR_VALUE, "Should not see", 1);
    if(false) assert_raise_return(false, ERR_VALUE, 1);
    if(false) assert_raisem_return(false, ERR_VALUE, "Should not see", 1);
    break;
  default:
    return 0;
  }
  if(tobreak) return 0;
  else return 1;
  
error:
  if(tofail){
    return 0;
  }
  else{
    return 1;
  }
}

void loop(){
  int out;
  int failures = 0;
  EH_Serial.println("\n\n\n\n");
  EH_Serial.println("Testing Standard:");
  for(int n = 0; n <= T1_TESTS; n++){
    EH_Serial.flush();
    EH_Serial.print("Testcase:");
    EH_Serial.println(n);
    
    clrerr();
    out = test1(n);
    if(out){
      EH_Serial.println("*** FAILURE ***");
      failures++;
      failbuffer.put(n);
    }
    else{
      EH_Serial.println("*** Success ***");
    }
  }
  
  EH_Serial.println("##### Tests Done #####");
  EH_Serial.println(String("Results: ") + String(failures) + String(" failures out of: ") + String(T1_TESTS));
  if(failbuffer.getSize()) {
    EH_Serial.println(String("Failed cases:"));
  }
  
  while(failbuffer.getSize() > 0){
    EH_Serial.print((unsigned short)failbuffer.get());
    EH_Serial.print(", ");
  } 
  while(true);
}


