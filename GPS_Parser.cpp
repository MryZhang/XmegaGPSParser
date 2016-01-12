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
	hour = minute = seconds = year = month = day = 0;
	milliseconds = 0;
	latitude_fixed =longitude_fixed = 0;
	latitude = longitude = 0.0f;
	latitudeDegrees = longitudeDegrees = 0.0f;
	lat = lon = mag = 0;
	fixquality = satellites = 0;
	speed = angle = magvariation = HDOP = 0.0f;
	geoidheight = altitude = 0.0f;
};

GPS_Parser::GPS_Parser(USART_Data * data) : USART(data, false){
	//USART(data, false);
	fix_avail = false;
	time_avail = false;
	hour = minute = seconds = year = month = day = 0;
	milliseconds = 0;
	latitude_fixed =longitude_fixed = 0;
	latitude = longitude = 0.0f;
	latitudeDegrees = longitudeDegrees = 0.0f;
	lat = lon = mag = 0;
	fixquality = satellites = 0;
	speed = angle = magvariation = HDOP = 0.0f;
	geoidheight = altitude = 0.0f;
}

uint8_t GPS_Parser::parseHex(char c){
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

bool GPS_Parser::parseGPGGA(char * message){
	char degreebuff[10];
	int32_t degree;
	long minutes;
	char *p = message;
	p = strchr(p, ',')+1;
	float timef = atof(p);
	uint32_t time = timef;
	hour = time / 10000;
	minute = (time % 10000) / 100;
	seconds = (time % 100);

	milliseconds = fmod(timef, 1.0) * 1000;
	printf("Time: %d:%d:%d\n", hour, minute, seconds);

	// parse out latitude
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		strncpy(degreebuff, p, 2);
		p += 2;
		degreebuff[2] = '\0';
		degree = atol(degreebuff) * 10000000;
		strncpy(degreebuff, p, 2); // minutes
		p += 3; // skip decimal point
		strncpy(degreebuff + 2, p, 4);
		degreebuff[6] = '\0';
		minutes = 50 * atol(degreebuff) / 3;
		latitude_fixed = degree + minutes;
		latitude = degree / 100000 + minutes * 0.000006F;
		latitudeDegrees = (latitude-100*int(latitude/100))/60.0;
		latitudeDegrees += int(latitude/100);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		if (p[0] == 'S') latitudeDegrees *= -1.0;
		if (p[0] == 'N') lat = 'N';
		else if (p[0] == 'S') lat = 'S';
		else if (p[0] == ',') lat = 0;
		else return false;
	}

	// parse out longitude
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		strncpy(degreebuff, p, 3);
		p += 3;
		degreebuff[3] = '\0';
		degree = atol(degreebuff) * 10000000;
		strncpy(degreebuff, p, 2); // minutes
		p += 3; // skip decimal point
		strncpy(degreebuff + 2, p, 4);
		degreebuff[6] = '\0';
		minutes = 50 * atol(degreebuff) / 3;
		longitude_fixed = degree + minutes;
		longitude = degree / 100000 + minutes * 0.000006F;
		longitudeDegrees = (longitude-100*int(longitude/100))/60.0;
		longitudeDegrees += int(longitude/100);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		if (p[0] == 'W') longitudeDegrees *= -1.0;
		if (p[0] == 'W') lon = 'W';
		else if (p[0] == 'E') lon = 'E';
		else if (p[0] == ',') lon = 0;
		else return false;
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		fixquality = atoi(p);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		satellites = atoi(p);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		HDOP = atof(p);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		altitude = atof(p);
	}

	p = strchr(p, ',')+1;
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		geoidheight = atof(p);
	}
	return true;
}

bool GPS_Parser::parseGPRMC(char * message){
	char degreebuff[10];
	int32_t degree;
	long minutes;
	// found RMC
	char *p = message;

	// get time
	p = strchr(p, ',')+1;
	float timef = atof(p);
	uint32_t time = timef;
	hour = time / 10000;
	minute = (time % 10000) / 100;
	seconds = (time % 100);

	milliseconds = fmod(timef, 1.0) * 1000;

	p = strchr(p, ',')+1;
	// Serial.println(p);
	if (p[0] == 'A')
		fix_avail = true;
	else if (p[0] == 'V')
		fix_avail = false;
	else
		return false;

	// parse out latitude
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		strncpy(degreebuff, p, 2);
		p += 2;
		degreebuff[2] = '\0';
		long degree = atol(degreebuff) * 10000000;
		strncpy(degreebuff, p, 2); // minutes
		p += 3; // skip decimal point
		strncpy(degreebuff + 2, p, 4);
		degreebuff[6] = '\0';
		long minutes = 50 * atol(degreebuff) / 3;
		latitude_fixed = degree + minutes;
		latitude = degree / 100000 + minutes * 0.000006F;
		latitudeDegrees = (latitude-100*int(latitude/100))/60.0;
		latitudeDegrees += int(latitude/100);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		if (p[0] == 'S') latitudeDegrees *= -1.0;
		if (p[0] == 'N') lat = 'N';
		else if (p[0] == 'S') lat = 'S';
		else if (p[0] == ',') lat = 0;
		else return false;
	}

	// parse out longitude
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		strncpy(degreebuff, p, 3);
		p += 3;
		degreebuff[3] = '\0';
		degree = atol(degreebuff) * 10000000;
		strncpy(degreebuff, p, 2); // minutes
		p += 3; // skip decimal point
		strncpy(degreebuff + 2, p, 4);
		degreebuff[6] = '\0';
		minutes = 50 * atol(degreebuff) / 3;
		longitude_fixed = degree + minutes;
		longitude = degree / 100000 + minutes * 0.000006F;
		longitudeDegrees = (longitude-100*int(longitude/100))/60.0;
		longitudeDegrees += int(longitude/100);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		if (p[0] == 'W') longitudeDegrees *= -1.0;
		if (p[0] == 'W') lon = 'W';
		else if (p[0] == 'E') lon = 'E';
		else if (p[0] == ',') lon = 0;
		else return false;
	}
	// speed
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		speed = atof(p);
	}

	// angle
	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		angle = atof(p);
	}

	p = strchr(p, ',')+1;
	if (',' != *p)
	{
		uint32_t fulldate = atof(p);
		day = fulldate / 10000;
		month = (fulldate % 10000) / 100;
		year = (fulldate % 100);
	}
	// we dont parse the remaining, yet!
	return true;
}

void parseGPRMA(){

}

bool GPS_Parser::readNMEA(){
	char buffer[127];
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
					// Code snippets borrowed from https://github.com/adafruit/Adafruit-GPS-Library/blob/master/Adafruit_GPS.cpp
					// Why re-invent what's already been invented..
					type = "GPGGA";
					if(parseGPGGA(buffer)){
						printf("GPGGA message successfully parsed.\n");
						printf("Time: %d:%d:%d\n", hour, minute, seconds);
						printf("Lat %f Lon %f\n", latitudeDegrees, longitudeDegrees);
						if(fixquality == 1){
							printf("Got a fix!\n");
						}
						printf("Fix Quality: %d\n", fixquality);
					} else {
						printf("Error parsing GPGGA message.");
					}
				} else if (strstr(buffer, "$GPRMA")){
					type = "GPRMA";
				} else if (strstr(buffer, "$GPRMC")){
					type = "GPRMC";
					if(parseGPRMC(buffer)){
						printf("GPRMC message successfully parsed.");
					} else {
						printf("Error parsing GPRMC message.");
					}
				} else {
					type = "other";
				}
				//char type[7];
				//memcpy(type, buffer, 7);
				//type[6] = '\0';

				printf("Type: %s\n", type);

				const char * endPtr = strstr(buffer, "*");
				uint8_t msg_len = bufPos-4;
				char message[msg_len];
				//printf("Message length: ")
				if(fixquality > 0){
					printf("Fix available!\n");
					break;
				} else {
					printf("Fix not avaliable\n");
				}
				memcpy(message, buffer, msg_len);
				printf("Message: %s\n", message);
				//printf("Original msg: %s\n", buffer);
				break;
			}

		}
	}

	return false;

}

bool GPS_Parser::fixAvailable(){
	return fix_avail;
}
