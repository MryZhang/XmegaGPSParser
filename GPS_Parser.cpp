/*
 * NMEA_Parser.cpp
 *
 *  Created on: 15 Dec 2015
 *      Author: leo
 */
#include "GPS_Parser.h"
#include "USART.h"
#include <string.h>

// empty constructor to keep compiler happy..
GPS_Parser::GPS_Parser() : USART(){
	time_avail = false;
	fix_avail = false;
};

GPS_Parser::GPS_Parser(USART_Data * data) : USART(data, false){
	//USART(data, false);
	fix_avail = false;
	time_avail = false;
}

bool GPS_Parser::readNMEA(){
	char buffer[255];
	char c;
	unsigned int bufPos = 0;

	while(1){
		c = this->GetChar();
		buffer[bufPos++] = c;
		printf("%c", c);
		if(c == 0x0A || c == 0 || bufPos > 255){
			break;
		}
	}

	if(bufPos > 3){
		printf("00: %c\n", buffer[bufPos]);
		printf("-1: %c\n", buffer[bufPos-1]);
		printf("-2: %c\n", buffer[bufPos-2]);
		printf("-3: %c\n", buffer[bufPos-3]);
		printf("-4: %c\n", buffer[bufPos-4]);
		if(buffer[bufPos-4] == '*'){
			printf("A NMEA message!\n");
			char type[7];
			memcpy(type, buffer, 7);
			type[6] = '\0';
			printf(type);
		}
	}


	return false;

}

bool GPS_Parser::fixAvailable(){
	return fix_avail;
}
