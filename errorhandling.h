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
// Serial.println(user_int);
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
//   if A is false, raise(E) then print message (like Serial.print)
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


#ifndef __debug_h__
#define __debug_h__

#include <errno.h>
#include <string.h>

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

#define ERR_TYPE          20 // TypeErr
#define ERR_VALUE         21 // ValueErr
#define ERR_ASSERT        22 // AssertErr

#define ERR_NONEW         253 // NoNew -- error already printed
#define ERR_EMPTY         254  //nothing printed
#define ERR_UNKNOWN       255  //unknown

#define LOGV_DEBUG 50
#define LOGV_INFO 40
#define LOGV_ERROR 30

void DBG_test();
extern int derr;
extern char *errmsg;
extern char *DBG_CLEAR_ERROR_MSG;

#define assert(A) if(!(A)) {errno=ERR_ASSERT; derr = ERR_ASSERT; log_err(); Serial.println(); goto error;}

#define raise(E) errno = (E); derr = (E); log_err(); Serial.println(); goto error;

#define assert_raise(A, E) if(!(A)) {raise((E))}

#define raisem(E, M, ...) errno = (E); derr = (E); log_err(); Serial.println((M), ##__VA_ARGS__); goto error;

#define assert_raisem(A, E, M, ...) if(!(A)) {raisem((E), (M), ##__VA_ARGS__)}

#define noerr() if(derr) goto error;

#define noerr_log() if(derr) {log_err(); Serial.println(); goto error;}

void clrerr();

#define clrerr_log() clrerr(); errmsg = DBG_CLEAR_ERROR_MSG; log_err(); Serial.println();

#ifdef DEBUG
void DBG_printerrno();
void dbg_printinfo(char *file, unsigned int line);


#define DEBUG
//set log level to debug if programmer didn't set it.
#ifndef LOGLEVEL
#define LOGLEVEL LOGV_DEBUG
#endif

// Only log at the proper level.
#if LOGLEVEL >= LOGV_DEBUG
void DBG_start_debug(char *file, unsigned int line);
#define debug(M, ...) DBG_start_debug(__FILE__, __LINE__); Serial.println((M), ##__VA_ARGS__);
#else
#define debug(M, ...) 
#endif

#if LOGLEVEL >= LOGV_INFO
void DBG_start_info(char *file, unsigned int line);
#define log_info(M, ...) DBG_start_info(__FILE__, __LINE__); Serial.println((M), ##__VA_ARGS__);
#else
#define log_info(M, ...) 
#endif

#if LOGLEVEL >= LOGV_ERROR
void DBG_log_err(char *file, unsigned int line);
#define log_err() DBG_log_err(__FILE__, __LINE__)
#else
#define log_err(...)
#endif

#else
#define debug(M, ...)
#define log_info(M, ...)
#define log_err()
#endif

#endif



