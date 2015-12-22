/*
 * NMEA_Parser.cpp
 *
 *  Created on: 15 Dec 2015
 *      Author: leo
 */
#include "GPS_Parser.h"
#include "USART.h"
#include <string.h>
#include <math.h>

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

uint8_t parseHex(char c){
	if (c < '0')
	  return 0;
	if (c <= '9')
	  return c - '0';
	if (c < 'A')
	   return 0;
	if (c <= 'F')
	   return (c - 'A')+10;
	// if (c > 'F')
	return 0;
}

bool GPS_Parser::readNMEA(){
	char buffer[255];
	char c;
	unsigned int bufPos = 0;

	while(1){
		c = this->GetChar();
		buffer[bufPos++] = c;
		//printf("%c", c);
		if(buffer[bufPos-4] == '*'){
			uint16_t sum = parseHex(buffer[bufPos-3]) * 16;
			sum += parseHex(buffer[bufPos-2]);

			// check checksum
			for (uint8_t i=2; i < (bufPos-4); i++) {
				sum ^= buffer[i];
			}
			if (sum != 0) {
				// bad checksum :(
				printf("Invalid packet.\n");
				return false;
			} else {
				printf("A valid NMEA message!\n");
				const char * type;
				if(strstr(buffer, "$GPGGA")){
					type = "GPGGA";
					// get time
					char *p = buffer;
					p = strchr(p, ',')+1;
					float timef = atof(p);
					uint32_t time = timef;
					hour = time / 10000;
					minute = (time % 10000) / 100;
					seconds = (time % 100);

					milliseconds = fmod(timef, 1.0) * 1000;
					printf("Time: %d:%d:%d\n", hour, minute, seconds);
				} else if (strstr(buffer, "$GPRMA")){
					type = "GPRMA";
				} else if (strstr(buffer, "$GPRMC")){
					type = "GPRMC";
				} else {
					type = "other";
				}
				//char type[7];
				//memcpy(type, buffer, 7);
				//type[6] = '\0';

				printf("Type: %s\n", type);
				char message[32];
				const char * endPtr = strstr(buffer, "*");
				uint8_t msg_len = bufPos-4;
				//printf("Message length: ")
				memcpy(message, buffer, msg_len);
				printf("Message: %s\n", message);
				//printf("Original msg: %s\n", buffer);
				break;
			}

		}
		/*if(c == 0x0A || c == 0 || bufPos > 255){
			break;
		}*/
	}
	/*
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
	} */

	return false;

}

bool GPS_Parser::fixAvailable(){
	return fix_avail;
}
