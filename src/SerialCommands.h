/*--------------------------------------------------------------------
Author		: Pedro Pereira
License		: BSD
Repository	: https://github.com/ppedro74/Arduino-SerialCommands
--------------------------------------------------------------------*/

//#define SERIAL_COMMANDS_DEBUG

#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifndef ARDUINO

//workarounds with stdio for uses off-Arduino, initially focused on stdio
#include <stdio.h>
#include <iostream>
#include <cstring>
#include "getch.c"
//#include "kbhit.c"
//#include <conio>

#define DEC    10
#define boolean  bool
#define size_t   int
#define int8_t   char
#define uint8_t  unsigned char
#define int16_t  int

class Stream {
  public:	  
    //https://github.com/esp8266/Arduino/blob/master/cores/esp8266/Stream.h
    //Many alternate print arguments supported (e.g.) here:
    //https://github.com/esp8266/Arduino/blob/master/cores/esp8266/Print.h
    //Initially, here we just support those invoked by SerialCommand

    //int read()      {return getchar();}
    int available() {return 1;} //punting initially

    int read()      {return getche();}
    int available() {return _kbhit();}
    size_t print(const char str[])   {printf("%s", str);   return 1;}
    size_t print(int i, int r= DEC)  {printf("%i", i);     return 1;}
    size_t print(char c)             {printf("%c", c);     return 1;}
    size_t println(const char str[]) {printf("%s\n", str); return 1;}
};

//Stream *SerialProxy = new Stream();
#endif

typedef enum ternary 
{
	SERIAL_COMMANDS_SUCCESS = 0,
	SERIAL_COMMANDS_ERROR_NO_SERIAL,
	SERIAL_COMMANDS_ERROR_BUFFER_FULL
} SERIAL_COMMANDS_ERRORS;

class SerialCommands;

typedef class SerialCommand SerialCommand;
class SerialCommand
{
public:
	SerialCommand(const char* cmd, void(*func)(SerialCommands*), bool one_k=false)
		: command(cmd),
		function(func),
		next(NULL),
		one_key(one_k)
	{
	}

	const char* command;
	void(*function)(SerialCommands*);
	SerialCommand* next;
	bool one_key;
};

class SerialCommands
{
public:
	SerialCommands(Stream* serial, char* buffer, int16_t buffer_len, const char* term = "\r\n", const char* delim = " ") :
		serial_(serial),
		buffer_(buffer),
		buffer_len_(buffer!=NULL && buffer_len > 0 ? buffer_len - 1 : 0), //string termination char '\0'
		term_(term),
		delim_(delim),
		default_handler_(NULL),
		buffer_pos_(0),
		last_token_(NULL), 
		term_pos_(0),
		commands_head_(NULL),
		commands_tail_(NULL),
		onek_cmds_head_(NULL),
		onek_cmds_tail_(NULL),
		commands_count_(0),
		onek_cmds_count_(0)
	{
	}


	/**
	 * \brief Adds a command handler (Uses a linked list)
	 * \param command 
	 */
	void AddCommand(SerialCommand* command);

	/**
	 * \brief Checks the Serial port, reads the input buffer and calls a matching command handler.
	 * \return SERIAL_COMMANDS_SUCCESS when successful or SERIAL_COMMANDS_ERROR_XXXX on error.
	 */
	SERIAL_COMMANDS_ERRORS ReadSerial();

	/**
	 * \brief Returns the source stream (Serial port)
	 * \return 
	 */
	Stream* GetSerial();
	
	/**
	 * \brief Attaches a Serial Port to this object 
	 * \param serial 
	 */
	void AttachSerial(Stream* serial);
	
	/**
	 * \brief Detaches the serial port, if no serial port nothing will be done at ReadSerial
	 */
	void DetachSerial();
	
	/**
	 * \brief Sets a default handler can be used for a catch all or unrecognized commands
	 * \param function 
	 */
	void SetDefaultHandler(void(*function)(SerialCommands*, const char*));
	
	/**
	 * \brief Clears the buffer, and resets the indexes.
	 */
	void ClearBuffer();
	
	/**
	 * \brief Gets the next argument
	 * \return returns NULL if no argument is available
	 */
	char* Next();

private:
	Stream* serial_;
	char* buffer_;
	int16_t buffer_len_;
	const char* term_;
	const char* delim_;
	void(*default_handler_)(SerialCommands*, const char*);
	int16_t buffer_pos_;
	char* last_token_;
	int8_t term_pos_;
	SerialCommand* commands_head_;
	SerialCommand* commands_tail_;
	SerialCommand* onek_cmds_head_;
	SerialCommand* onek_cmds_tail_;
	uint8_t commands_count_;
	uint8_t onek_cmds_count_;

	/**
	 * \brief Tests for any one_key command and execute it if found
	 * \return false if this was not a one_key command, true otherwise with
	 *		   also clearing the buffer
	 **/
	bool CheckOneKeyCmd();
};

#endif
