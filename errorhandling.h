/*

 LICENSE: This code is released under the GNU GPL v3 or greater. For more info see:
 <http://www.gnu.org/licenses/gpl.html>
 Copyright: Garrett Berg cloudformdesign.com garrett@cloudformdesign.com
 Loosly based on code from the fantastic "Learn C the Hard Way",  Zed A. Shaw
 http://c.learncodethehardway.org/book/ex20.html
*/ 
// Arduino Debug Library
// This is the arduino debug library, meant to make debugging and error 
// handling easier.
// 
// Intended Use:
// There are many times where you want to do error checking and simultaniously provide debugging.
// This library helps significantly. The intended use is to make an assertion for your failure
// point, and then in the "error:" goto define errno and derr to be what your values are.
// 
// You then use noerr_log() to follow the stack trace down, so you can see what called what,
// while simultaniously doing the necessary error checking.
// 
//   Top Level Function
// void print_user_int(){
// int user_int = get_user_int(); // expects to get an integer from the user
// noerr_log();  // if there was an error, log it and go to error:
// EH_Serial.println(user_int);
// return;
// error:
// clrerr_log(); //clears error flags and logs that it did so.
// return;
// }
// 
// Output:
// If the user puts in an int, it will simply print it back.
// 
// If they put in a different character, it will print out the
// following error
// [ERR](20 Example.ino:13)| (20:TypeErr)[char in hex]
// [ERR](20 Example.ino:3)| (253:NoNew)
// [ERR](0 Example.ino:7)| (0:NoErr)Cleared Error
// 
// The first number is derr -- the value that is checked by noerr
// and which you should check to make sure there hasn't been an
// error.
// Then there is the file and line number
// Finally, there is a verbose printout of errno (with a message
// if one is included). Note that errno is set to ERR_NONEW 
// after it has been printed.
// 
// Preprossessor Definitions
// #define DEBUG ::
//   define DEBUG before importing this module to print things, otherwise error checking, etc will be silent.
// 
// 
// Global Variables:
// errno :: from "errno.h". Specifys error type.
// derr  :: debug error, specifys error type. Kept separate for printing convienience
// 
// Macro Overview:
// Logging functions
// debug(M, ...) ::
//   prints out: [DBG](derr file:line)|M
// log_info(M, ...) ::
//   prints out: [INFO](derr file:line)|M
// log_err(M, ...) ::
//   prints out [ERR](derr file:line)|(strerrno:errno)M
//   Note automatically called by all below functions except noerr and clrerr...
//
// assert(A) ::
//   Assert that the value is true. If it is not true, then derr and errno = ERR_ASSERT
//   Also logs error with message "AS"
//   Note: requires "error:" defined for goto
//
// raise(E) ::
//   Raises the error given, logging it.
//   Note: requires "error:" defined for goto
// 
// assert_raise(A, E) ::
//   if A is false, raise(E)
//   Note: requires "error:" defined for goto
// 
// assert_raisem(A, E, M, ...) ::
//   if A is false, raise(E) then print message (like EH_Serial.print)
//   Note: requires "error:" defined for goto
// 
// noerr() ::
//   same as assert(!derr)
//   Note: requires "error:" defined for goto
// 
// noerr_log() ::
//   same as noerr() but provides a log as well with message "NE"
//   Note: requires "error:" defined for goto
// 
// clrerr() ::
//   clears error specifiers (derr = 0; errno = 0)
// 
// clrerr_log() ::
//   also logs

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

#define ERR_NOERR         0 // NoErr -- no error has occured
#define ERR_BASE          1 // BaseErr
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

#define ERR_CLEARED       252 // "Cleared Error" used by clrerr_log
#define ERR_NONEW         253 // NoNew -- error already printed
#define ERR_EMPTY         254  //nothing printed
#define ERR_UNKNOWN       255  //unknown

#define LOGV_DEBUG 50
#define LOGV_INFO 40
#define LOGV_ERROR 30

void EH_test();
extern unsigned short derr;
extern unsigned short errno;
extern char *errmsg;
extern char *EH_CLEAR_ERROR_MSG;

void clrerr();
void seterr(unsigned short error);

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

#ifdef LOGLEVEL
#define DEBUG
#endif

#ifdef DEBUG
#define DEBUG
void EH_printerrno();
void EH_printinfo(char *file, unsigned int line);

#ifndef LOGLEVEL
#define LOGLEVEL LOGV_DEBUG
#endif

// Only log at the proper level.
#if LOGLEVEL >= LOGV_DEBUG
void EH_start_debug(char *file, unsigned int line);
#define debug(M, ...) EH_DW(EH_start_debug(__FILE__, __LINE__); EH_Serial.println((M), ##__VA_ARGS__);)
#else
#define debug(M, ...) 
#endif

#if LOGLEVEL >= LOGV_INFO
void EH_start_info(char *file, unsigned int line);
#define log_info(M, ...) EH_DW(EH_start_info(__FILE__, __LINE__); EH_Serial.println((M), ##__VA_ARGS__);)
#else
#define log_info(M, ...) 
#endif

#if LOGLEVEL >= LOGV_ERROR
  void EH_log_err(char *file, unsigned int line);
  #define EH_ST_raisem(E, ...) seterr(E); EH_log_err(__FILE__, __LINE__); EH_Serial.println(__VA_ARGS__)
  #define log_err(...) EH_DW(EH_log_err(__FILE__, __LINE__); EH_Serial.println(__VA_ARGS__);)
  #define clrerr_log() EH_DW(seterr(ERR_CLEARED); log_err(); clrerr();)

#else
  #define EH_ST_raisem(E, ...)
  #define log_err(...)
  #define clrerr_log() clrerr()
#endif

#else

#define debug(M, ...)
#define log_info(M, ...)
#define log_err(...)
#define clrerr_log() clrerr()
#define EH_ST_raisem(E, ...)
#endif

// Using pt (protothreads) library with debug.
// You can use the below functions OR you can define your own 
// error handling.
// If you do error handling, make PT_ERROR_OCCURED your last line.
//  (returns PT_ERROR)

#ifdef DEBUG
#define PT_RAISE(pt, E) derr = (E); if(derr != pt->error){ errno=ERR_ASSERT; log_err(); EH_Serial.println();} return PT_ERROR
#endif

#define PT_ERROR_OCCURED return PT_ERROR

// sets the pt location so that when it fails, it starts there again
#define PT_ERROR_TRY(pt) PTX_time = 0; LC_SET(pt)

// if PT_ASSERT fails, returns. On next call continues at place last set (use PT_ERROR_TRY)

#define PT_NOERR(pt) if(derr) return PT_ERROR

#endif



