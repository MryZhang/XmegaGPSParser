/*
 * NMEA_Parser.h
 *
 *  Created on: 15 Dec 2015
 *      Author: leo
 */

#ifndef GPS_PARSER_H_
#define GPS_PARSER_H_

#include <avr/io.h>
#include "USART.h"
// Note: time.h requires newer AVR-GCC such as one found in http://swfltek.com/arduino/build/index.html
// If using avr-eclipse, or makefiles in Linux/Unix/Mac. Should be easy to substitute.
#include "time.h"

class GPS_Parser : public USART{
private:
	bool time_avail, fix_avail;
	struct tm time_struct;
	uint8_t hour, minute, seconds, year, month, day;
	uint16_t milliseconds;
	int32_t latitude_fixed, longitude_fixed;
	float latitude, longitude;
	float latitudeDegrees, longitudeDegrees;
	char lat, lon, mag;
	uint8_t fixquality, satellites;
	float speed, angle, magvariation, HDOP;
	float geoidheight, altitude;
	float lastLatitudeDegrees, lastLongitudeDegrees;
	bool newLatitudeDegrees, newLongitudeDegrees;

	uint8_t parseHex(char c);

public:
	GPS_Parser();
	GPS_Parser(USART_Data * data)  ;

	bool readNMEA();

	void praseRMC();
	void parseGGA();
	bool parseGPGGA(char * message);
	bool parseGPRMC(char * message);

	bool timeAvailable();
	bool fixAvailable();

	float getLatitude();
	float getLongitude();
	bool newPositionAvailable();
	struct tm  * getTimeStruct();
	unsigned int getMinute();
	unsigned int getHour();
	unsigned int geteDOM();
	unsigned int getMonth();
	unsigned int getYear();
};



#endif /* GPS_PARSER_H_ */
