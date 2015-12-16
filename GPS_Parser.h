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
#include "time.h"

class GPS_Parser : public USART{
private:
	bool time_avail;
	bool fix_avail;
	struct tm time_struct;

public:
	GPS_Parser();
	GPS_Parser(USART_Data * data)  ;

	bool readNMEA();

	void praseRMC();
	void parseGGA();

	bool timeAvailable();
	bool fixAvailable();

	double getLatitude();
	double getLongitude();
	struct tm  * getTime_struct();
	unsigned int getMinute();
	unsigned int getHour();
	unsigned int geteDOM();
	unsigned int getMonth();
	unsigned int getYear();
};



#endif /* GPS_PARSER_H_ */
