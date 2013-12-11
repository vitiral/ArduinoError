/*
 LICENSE: This code is released under the GNU GPL v3 or greater. For more info see:
 <http://www.gnu.org/licenses/gpl.html>
 Copyright: Garrett Berg cloudformdesign.com garrett@cloudformdesign.com
 Loosly based on code from the fantastic "Learn C the Hard Way",  Zed A. Shaw
 http://c.learncodethehardway.org/book/ex20.html
*/ 
// #####  Arduino Debug Library
// This is the arduino debug library, meant to make debugging and error 
// handling easier.
// 
// ### Intended Use:
// There are many times where you want to do error checking and simultaniously provide debugging.
// This library helps significantly. Errors are automatically printed if DEBUG is defined.
// This library offers two ways to deal with errors, documented below:
// return checks     -- these do all your logging for you and set derr. (the global error variable).
//                        They then return R.
//                        these are intended for "simple use" cases, where you do not need to
//                        do any error handling (like send special error messages, close ports,
//                        etc)
// goto error checks -- these do your logging, etc and then "goto error;" where you can do error
//                        handling like closing ports, etc.
// goto error checks -- these do your logging, etc and then "goto error;" where you can do error
//                        handling like closing ports, etc.
// 
// You then use noerr_log_return() to follow the stack trace down, so you can see what called what. This
//   helps you keep track of where your code is failing.
// 
// Preprossessor Definitions
// #define DEBUG ::
//   define DEBUG before importing this module to print things, otherwise error checking, etc will be silent.
//   if DEBUG is defined and LOGLEVEL is not, LOGLEVEL == 50
// #defien LOGLEVEL ::
//   LOGLEVEL is various values up to 50. If loglevel is defined, DEBUG is automatically defined.
//   if LOGLEVEL is...
//      >= 50       all messages printed (debug, info, error)
//      <50         debug not printed
//      <40         info not printed
//      <30         nothing is printed
//
// Alternate Serial Ports:
//   You can use a different serial port from the hardware one, through the SoftwareSerial
//     library. Simple initilize your SoftwareSerial class and pass it to EH_config.
//   
//   Example:
//      mysoft = SoftwareSerial(10, 11); // RX, TX
//      void setup(){
//          mysoft.begin(57600);        //57600 is the recommended speed because the library can delay other code
//          EH_config(mysoft);
//          ... rest of your stuff
//      }
//
//   TODO: be able to use other hardware ports.
// 
// Global Variables:
//   errno :: specifies error type. String format is automatically printed with log_err()
//   derr  :: main error. This is what specifies that an error occured (non-zero == error)
// 
// ###  Macro Overview:
// # Logging functions:
// debug(...) ::
//   prints out: [DBG](derr file:line)|...
// log_info(...) ::
//   prints out: [INFO](derr file:line)|...
// log_err(...) ::
//   prints out [ERR](derr file:line)|(strerrno:errno)|...
//   Note automatically called by all below functions except noerr and clrerr
//
// # Error handling Functions:
// assert(A) ::
//   Assert that the value is true. If it is not true, then derr and errno = ERR_ASSERT
//   Also logs error with message "AS"
//   Note: requires "error:" defined for goto
//
// raise(E, ...) ::
//   Raises the error given, logging it.
//   Also can print a message
//   Note: requires "error:" defined for goto
// 
// assert_raise(A, E, ...) ::
//   if A is false, raise(E)
//   Also can print a message
//   Note: requires "error:" defined for goto
// 
// iferr_catch() ::
//   if there is an error, "goto error"
//   Note: requires "error:" defined for goto
//
// iferr_log_catch() ::
//   same as above, but also logs error.
//   Note: requires "error:" defined for goto
// 
// [fun]_return(..., return_val) ::
//   The above functions have a "return" variety, where they return the return_val
//   instead of "goto error"
//   They are: assert_return(A, R)  raise_return(E, R)  assert_raise_return(A, E, R)  
//             raisem_return(E, M, R) /*where M is a message*/ 
//             assert_raisem_return(A, E, M, R)
//             iferr_return(R)  iferr_log_return(R)
//   note: in void functions, R should not be included (i.e. assert_return(A))
//
// clrerr() ::
//   clears error specifiers (derr = 0; errno = 0)
// 
// clrerr_log() ::
//   also logs

#define ERR_NOERR         0 // NoErr -- no error has occured
#define ERR_BASE          1 // BaseErr -- general error
#define ERR_TIMEOUT       2 // TimeoutErr
#define ERR_SERIAL        3 // SerialErr
#define ERR_SPI           4 // Spierr
#define ERR_I2C           5 // I2cErr
#define ERR_COMMUNICATION 6 // ComErr
#define ERR_CONFIG        7 // ConfigErr
#define ERR_PIN           8 // PinErr
#define ERR_INPUT         9 // InputErr

#define ERR_TYPE          50 // TypeErr
#define ERR_VALUE         51 // ValueErr
#define ERR_ASSERT        52 // AssertErr
#define ERR_TESTFAIL      53 // TestFail

#define ERR_CLEARED       252 // "Cleared Error" used by clrerr_log and is then cleared
#define ERR_NONEW         253 // NoNew -- error already printed
#define ERR_EMPTY         254 // nothing printed, still an error
#define ERR_UNKNOWN       255 // unknown error

#define LOGV_DEBUG 50
#define LOGV_INFO 40
#define LOGV_ERROR 30
#define LOGV_SILENT 0

#ifndef __errorhandling_h__
#define __errorhandling_h__

#include <SoftwareSerial.h>
#include <string.h>
#include <inttypes.h>
#include <Stream.h>

#define EH_STD_SERIAL 0
#define EH_SOFT_SERIAL 1

class EH_Serial_class : public Stream
{
private:
  // per object data
  uint8_t _mode;
  SoftwareSerial *_soft;
 
public:
  // public methods
  EH_Serial_class();
  //EH_Serial_class(uint8_t receivePin, uint8_t transmitPin, bool inverse_logic = false);
  ~EH_Serial_class();
//  void begin(long speed);
//  bool listen();
//  void end();
//  bool isListening() { return this == active_object; }
//  bool overflow() { bool ret = _buffer_overflow; _buffer_overflow = false; return ret; }
//  bool overflow();

  void config_std();
  void config_soft(SoftwareSerial *soft);
  int peek();
  virtual size_t write(uint8_t byte);
  virtual int read();
  virtual int available();
  virtual void flush();
  
  using Print::write;
};

extern EH_Serial_class EH_Serial;
#define EH_config_std            EH_Serial.config_std()
#define EH_config_soft(soft)     EH_Serial.config_soft(&soft)

void EH_test();
extern uint8_t derr;
extern uint8_t errno;
extern char *errmsg;
extern char *EH_CLEAR_ERROR_MSG;

void clrerr();
void seterr(uint8_t error);

#define EH_DW(code) do{code}while(0) //wraps in a do while(0) so that the syntax is correct.

#define raise(E, ...)                       EH_DW(EH_ST_raisem(E, __VA_ARGS__); goto error;)
#define assert(A)                           EH_DW(if(!(A)) {seterr(ERR_ASSERT); log_err(); goto error;})
#define assert_raise(A, E, ...)             EH_DW(if(!(A)) {raise((E), __VA_ARGS__);})

// These functions make the error label unnecessary
#define raise_return(E, ...)                    EH_DW(seterr(E); log_err(); return __VA_ARGS__;)
#define raisem_return(E, M, ...)                EH_DW(EH_ST_raisem(E, M); return __VA_ARGS__;)
#define assert_return(A, ...)                   EH_DW(if(!(A)) {seterr(ERR_ASSERT); log_err(); return __VA_ARGS__;})
#define assert_raise_return(A, E, ...)          EH_DW(if(!A){seterr(E); log_err(); return __VA_ARGS__;})
#define assert_raisem_return(A, E, M, ...)      EH_DW(if(!(A)) {EH_ST_raisem(E, M); return __VA_ARGS__;})


//#define iferr_return        if(derr) return 
//#define iferr_log_return    if(derr) {log_err();}  iferr_return
#define iferr_return(...)        EH_DW(if(derr) return __VA_ARGS__;)
#define iferr_log_return(...)    EH_DW(if(derr) {log_err(); return __VA_ARGS__;})
#define iferr_catch()            EH_DW(if(derr) goto error;)
#define iferr_log_catch()        EH_DW(if(derr) {log_err(); goto error;})

#ifdef DEBUG
  #ifndef LOGLEVEL
  #define LOGLEVEL LOGV_DEBUG
  #endif
#endif

#ifdef LOGLEVEL
  #if LOGLEVEL >= LOGV_ERROR
  extern uint8_t loglevel;
  #define EH_IFLL(LL,code) if(loglevel >= LL){code} 
  #endif
#else
  #define LOGLEVEL LOGV_SILENT
#endif

// Only log at the proper level.
#if LOGLEVEL >= LOGV_DEBUG
  void EH_start_debug(char *file, unsigned int line);
  #define debug(...) EH_DW(EH_IFLL(LOGV_DEBUG, EH_start_debug(__FILE__, __LINE__); EH_Serial.println(__VA_ARGS__);))
#else
  #define debug(...) 
#endif

#if LOGLEVEL >= LOGV_INFO
void EH_start_info(char *file, unsigned int line);
  #define log_info(...) EH_DW(EH_IFLL(LOGV_INFO, EH_start_info(__FILE__, __LINE__); EH_Serial.println(__VA_ARGS__);))
#else
  #define log_info(...) 
#endif

#if LOGLEVEL >= LOGV_ERROR
  void EH_log_err(char *file, unsigned int line);
  #define EH_ST_raisem(E, ...) seterr(E); EH_DW(EH_IFLL(LOGV_ERROR, EH_log_err(__FILE__, __LINE__); EH_Serial.println(__VA_ARGS__);))
  #define log_err(...)              EH_DW(EH_IFLL(LOGV_ERROR, EH_log_err(__FILE__, __LINE__); EH_Serial.println(__VA_ARGS__);))
  #define clrerr_log()              EH_DW(EH_IFLL(LOGV_ERROR, seterr(ERR_CLEARED); log_err(); clrerr();))
  void EH_printerrno();
  void EH_printinfo(char *file, unsigned int line);
  #define set_loglevel(LL) loglevel = LL
#else
  #define EH_ST_raisem(E, ...) seterr(E)
  #define log_err(...)
  #define clrerr_log() clrerr()
  #define set_loglevel(LL)
#endif


// Using pt (protothreads) library with debug.
// You can use the below functions OR you can define your own 
// error handling.
// If you do error handling, make PT_ERROR_OCCURED your last line.
//  (returns PT_ERROR)

#ifdef DEBUG000
#define PT_RAISE(pt, E) derr = (E); if(derr != pt->error){ errno=ERR_ASSERT; log_err(); EH_Serial.println();} return PT_ERROR


#define PT_ERROR_OCCURED return PT_ERROR

// sets the pt location so that when it fails, it starts there again
#define PT_ERROR_TRY(pt) PTX_time = 0; LC_SET(pt)

// if PT_ASSERT fails, returns. On next call continues at place last set (use PT_ERROR_TRY)

#define PT_NOERR(pt) if(derr) return PT_ERROR
#endif
#endif



