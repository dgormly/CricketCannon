/*
**************************************************************************
Copyright 2017 Team 30 - ENGG2800 (2017) - The University of Queensland

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**************************************************************************

**************************************************************************
File Name	: 'main.c'
Title		: LED clock Firmware for the ATmega328
Author		: Team 30 - ENGG2800 (2017) - The University of Queensland
Created		: 02/09/2017
Revised		: 22/11/2017
Version		: 1.0
Target MCU	: ATMEL AVR Series

This code is distributed under the GNU Public License
	which can be found at http://www.gnu.org/licenses/gpl.txt
**************************************************************************

**************************************************************************
Copyright 2017 Team 30 - ENGG2800 (2017) - The University of Queensland

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following
conditions are met:

	1. Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in the
	documentation and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**************************************************************************
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "light_ws2812.h"
#include "i2cmaster.h"
#include "uart.h"

//LDR defines
#define THRES 450

//LED defines
#define T1 0
#define T2 1
#define T3 2
#define T4 3
#define T5 4
#define T6 5
#define T7 6
#define T8 7
#define T9 8
#define T10 9
#define T11 10
#define T12 11
#define MAX_LED_L 12
#define MAX_LED_S 4
#define HOURS 1
#define MINUTES 2
#define SUB_MINUTES 3
#define HIGH 1
#define LOW 0
#define BRIGHT 3
#define DULL 9
#define OFF 000
#define TRUE 1
#define FALSE 0
#define TIME_AM 0
#define TIME_PM 1
#define COLOUR_DAY_RED 217
#define COLOUR_DAY_GREEN 22
#define COLOUR_DAY_BLUE 54
#define COLOUR_NIGHT_BLUE 255
#define COLOUR_IDLE_RED 0
#define COLOUR_IDLE_GREEN 0
#define COLOUR_IDLE_BLUE 0
#define COLOUR_IDLE_OUT_RED 0
#define COLOUR_IDLE_OUT_GREEN 0
#define COLOUR_IDLE_OUT_BLUE 0
#define COLOUR_SUNNY_RED 255
#define COLOUR_SUNNY_GREEN 250
#define COLOUR_SUNNY_BLUE 33
#define COLOUR_RAIN_RED 000
#define COLOUR_RAIN_GREEN 000
#define COLOUR_RAIN_BLUE 255
#define COLOUR_STORM_RED 142
#define COLOUR_STORM_GREEN 000
#define COLOUR_STORM_BLUE 255
#define COLOUR_WINDY_RED 000

#define COLOUR_WINDY_GREEN 255
#define COLOUR_WINDY_BLUE 000
#define COLOUR_CLOUDY_RED 150
#define COLOUR_CLOUDY_GREEN 150
#define COLOUR_CLOUDY_BLUE 150

//RTC defines
#define MCP7940N_W 0xDE  // I2C Address for the RTC
#define MCP7940N_R 0xDF  // I2C Address for the RTC
#define REG_RTCSEC 0x00  // Register Address: Time Second
#define REG_RTCMIN 0x01  // Register Address: Time Minute
#define REG_RTCHOUR 0x02  // Register Address: Time Hour
#define REG_RTCWKDAY 0x03  // Register Address: Date Day of Week
#define REG_RTCDATE 0x04  // Register Address: Date Day
#define REG_RTCMTH 0x05  // Register Address: Date Month
#define REG_RTCYEAR 0x06  // Register Address: Date Year
#define REG_CONTROL 0x07
#define ALARM0_REG_RTCMIN 0x0B  // Register Address: Time Minute
#define ALARM0_REG_RTCWKDAY 0x0D  // Register Address: Date Day of Week
#define ALARM1_REG_RTCHOUR 0x13  // Register Address: Time Minute
#define ALARM1_REG_RTCWKDAY 0x14  // Register Address: Date Day of Week

//RTC global variables
unsigned char timeStamp[4];
unsigned char alarm[3];
int alarmFlag = FALSE;
int weatherSaved = -1;

//LED global variables
struct cRGB h[12];
struct cRGB m[12];
struct cRGB sm[4];
int brightness = LOW;

//Converts a decimal value into a binary coded decimal value
unsigned char dec_to_bcd_convert(unsigned char dec) {
	unsigned char returnValue = (dec / 10) * 16 + (dec % 10);
	return returnValue;
}

//Converts a binary coded decimal value into a decimal value
unsigned char bcd_to_dec_convert(unsigned char bcd) {
	unsigned char returnValue;
	returnValue = ((bcd & 0xF0) >> 4) * 10 + (bcd & 0x0F);
	return returnValue;
}

//Enables the analog to digital functions on the micro controller 
void adc_init() {
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

//Reads the voltage of a supplied pin and returns a digital representation
uint16_t adc_read(uint8_t pinNum) {
	pinNum &= 0b00000111;
	ADMUX = (ADMUX & 0xF8) | pinNum;
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	return (ADC);
}

//Buzzer Control. Takes the frequency and duration of the note to be played and then creates the required signal loop to make the corresponding sound wave
void play_note(int freqNum, float duration) {
	long int counter1;
	int loops;
	float counter2;
	float freq[10];
	float half;
	float wavelength;
	freq[0] = 932;
	freq[1] = 988;
	freq[2] = 1047;
	freq[3] = 1109;
	freq[4] = 1175;
	freq[5] = 1244;
	freq[6] = 1319;
	freq[7] = 1397;
	freq[8] = 1480;
	freq[9] = 1568;
	wavelength = (1 / freq[freqNum]) * 1000;
	loops = duration / wavelength;
	half = wavelength / 2;
	for (counter1 = 0; counter1 < loops; counter1++) {
		for (counter2 = 0; counter2 < half; counter2 = counter2 + 0.01) {
			_delay_us(1);
			PORTD |= (1 << PD5);
		}
		for (counter2 = 0; counter2 < half; counter2 = counter2 + 0.01) {	
			_delay_us(1);
			PORTD &= ~(1 << PD5);
		}	
	}
	return;
}

//Sets all the LED's colour values using the functions built into the light_ws2812 library
void update_leds() {
	cli();
	ws2812_setleds_pin(h, MAX_LED_L, _BV(PD4));
	ws2812_setleds_pin(m, MAX_LED_L, _BV(PD3));
	ws2812_setleds_pin(sm, MAX_LED_S, _BV(PD2));
	sei();
}

//Edits the colour values for the LED's in their structs to match the current brightness level.
void change_all_leds_brightness() {
	int strength;
	int counter;
	if (brightness == LOW) {
		strength = DULL;
		} else {
		strength = BRIGHT;
	}
	for (counter = 0; counter < 12; ++counter) {
		if (h[counter].r > OFF) {
			h[counter].r = h[counter].r / strength;
		}
		if (h[counter].g > OFF) {
			h[counter].g = h[counter].g / strength;
		}
		if (h[counter].b > OFF) {
			h[counter].b = h[counter].b / strength;
		}
		if (m[counter].r > OFF) {
			m[counter].r = m[counter].r / strength;
		}
		if (m[counter].g > OFF) {
			m[counter].g = m[counter].g / strength;
		}
		if (m[counter].b > OFF) {
			m[counter].b = m[counter].b / strength;
		}
	}
	for (counter = 0; counter < 4; ++counter) {
		if (sm[counter].r > OFF) {
			sm[counter].r = sm[counter].r / strength;
		}
		if (sm[counter].g > OFF) {
			sm[counter].g = sm[counter].g / strength;
		}
		if (sm[counter].b > OFF) {
			sm[counter].b = sm[counter].b / strength;
		}
	}
	update_leds();
	return;
}

//Compares the value returned from adc_read against the set threshold amount and changes the brightness global variable accordingly
void brightness_control() {
	uint16_t ldrVolt;
	ldrVolt = adc_read(0);
	if (ldrVolt > THRES) {
		if (brightness == HIGH) {
			return;
		} else {
			brightness = HIGH;
			change_all_leds_brightness();
			return;
		}
	} else {
		if (brightness == LOW) {
			return;
		} else {
			brightness = LOW;
			change_all_leds_brightness();
			return;
		}
	}
	return;
}

//Animation for the weather. Lights up the LED's in sequence
void circle_animation(int colourR, int colourG, int colourB, int ledLine, int clearLeds) {
	int clightRed;
	int clightGreen;
	int clightBlue;
	int counter;
	int ledNum;
	int strength;
	if (brightness == LOW) {
		clightRed = colourR / DULL;
		clightGreen = colourG / DULL;
		clightBlue = colourB / DULL;
		strength = DULL;
	} else {
		clightRed = colourR / BRIGHT;
		clightGreen = colourG / BRIGHT;
		clightBlue = colourB / BRIGHT;
		strength = BRIGHT;
	}
	for (counter = 0; counter < MAX_LED_L; ++counter) {
		if (clearLeds == TRUE) {
			h[counter].r = COLOUR_IDLE_OUT_RED / strength; h[counter].g = COLOUR_IDLE_OUT_GREEN / strength; h[counter].b = COLOUR_IDLE_OUT_BLUE / strength;
		}
		m[counter].r = COLOUR_IDLE_RED / strength; m[counter].g = COLOUR_IDLE_GREEN / strength; m[counter].b = COLOUR_IDLE_BLUE / strength;
	}
	for (counter = 0; counter < MAX_LED_S; ++counter) {
		sm[counter].r = COLOUR_IDLE_RED / strength; sm[counter].g = COLOUR_IDLE_GREEN / strength; sm[counter].b = COLOUR_IDLE_BLUE / strength;
	}
	update_leds();
	for (counter = 0; counter < MAX_LED_L; ++counter) {
		if (counter < 6) {
			ledNum = counter + 6;
		} else {
			ledNum = counter - 6;
		}
		if (ledLine == HOURS) {
			h[ledNum].r = clightRed; h[ledNum].g = clightGreen; h[ledNum].b = clightBlue;
			brightness_control();
			update_leds();
			_delay_ms(125);
		} else {
			m[ledNum].r = clightRed; m[ledNum].g = clightGreen; m[ledNum].b = clightBlue;
			brightness_control();
			update_leds();
			_delay_ms(125);
		}
	}
	return;
}

//Chooses the colour values and LED line to use to show the weather and then sends the values to the circle_animation function
void flash_weather(int weather) {
	brightness_control();
	if (weather == -1) {
		return;
	}
	if (weather == 0) {
		circle_animation(COLOUR_SUNNY_RED, COLOUR_SUNNY_GREEN, COLOUR_SUNNY_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_SUNNY_RED, COLOUR_SUNNY_GREEN, COLOUR_SUNNY_BLUE, MINUTES, FALSE);
	} else if (weather == 1) {
		circle_animation(COLOUR_SUNNY_RED, COLOUR_SUNNY_GREEN, COLOUR_SUNNY_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_WINDY_RED, COLOUR_WINDY_GREEN, COLOUR_WINDY_BLUE, MINUTES, FALSE);
	} else if (weather == 2) {
		circle_animation(COLOUR_CLOUDY_RED, COLOUR_CLOUDY_GREEN, COLOUR_CLOUDY_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_WINDY_RED, COLOUR_WINDY_GREEN, COLOUR_WINDY_BLUE, MINUTES, FALSE);
	} else if (weather == 3) {
		circle_animation(COLOUR_CLOUDY_RED, COLOUR_CLOUDY_GREEN, COLOUR_CLOUDY_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_CLOUDY_RED, COLOUR_CLOUDY_GREEN, COLOUR_CLOUDY_BLUE, MINUTES, FALSE);
	} else if (weather == 4) {
		circle_animation(COLOUR_CLOUDY_RED, COLOUR_CLOUDY_GREEN, COLOUR_CLOUDY_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_RAIN_RED, COLOUR_RAIN_GREEN, COLOUR_RAIN_BLUE, MINUTES, FALSE);
	} else if (weather == 5) {
		circle_animation(COLOUR_RAIN_RED, COLOUR_RAIN_GREEN, COLOUR_RAIN_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_RAIN_RED, COLOUR_RAIN_GREEN, COLOUR_RAIN_BLUE, MINUTES, FALSE);
	} else if (weather == 6) {
		circle_animation(COLOUR_WINDY_RED, COLOUR_WINDY_GREEN, COLOUR_WINDY_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_WINDY_RED, COLOUR_WINDY_GREEN, COLOUR_WINDY_BLUE, MINUTES, FALSE);
	} else if (weather == 7) {
		circle_animation(COLOUR_RAIN_RED, COLOUR_RAIN_GREEN, COLOUR_RAIN_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_WINDY_RED, COLOUR_WINDY_GREEN, COLOUR_WINDY_BLUE, MINUTES, FALSE);
	} else if (weather == 8) {
		circle_animation(COLOUR_STORM_RED, COLOUR_STORM_GREEN, COLOUR_STORM_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_STORM_RED, COLOUR_STORM_GREEN, COLOUR_STORM_BLUE, MINUTES, FALSE);
	} else if (weather == 9) {
		circle_animation(COLOUR_STORM_RED, COLOUR_STORM_GREEN, COLOUR_STORM_BLUE, HOURS, TRUE);
		circle_animation(COLOUR_WINDY_RED, COLOUR_WINDY_GREEN, COLOUR_WINDY_BLUE, MINUTES, FALSE);
	}
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCMIN);
	i2c_rep_start(MCP7940N_R);
	timeStamp[1] = bcd_to_dec_convert(0b01111111 & i2c_readNak());
	i2c_stop();
	return;
}

//Changes the colour values in the LED structs so that they match the current time
void change_leds(int ledLine, int ledNum) {
	int strength;
	int lightRed;
	int lightGreen;
	int lightBlue;
	int counter;
	int ledNumChange;
	brightness_control();
	if (brightness == LOW) {
		strength = DULL;
	} else {
		strength = BRIGHT;
	}
	if (timeStamp[3] == TIME_AM) {
		lightRed = COLOUR_DAY_RED;
		lightGreen = COLOUR_DAY_GREEN;
		lightBlue = COLOUR_DAY_BLUE;
	} else {
		lightRed = OFF;
		lightGreen = OFF;
		lightBlue = COLOUR_NIGHT_BLUE;
	}
	for (counter = 0; counter < MAX_LED_L; ++counter) {
		if (ledLine == 1) {
		h[counter].r = COLOUR_IDLE_OUT_RED / strength; h[counter].g = COLOUR_IDLE_OUT_GREEN / strength; h[counter].b = COLOUR_IDLE_OUT_BLUE / strength;
		}
		if (ledLine == 2) {
		m[counter].r = COLOUR_IDLE_RED / strength; m[counter].g = COLOUR_IDLE_GREEN / strength; m[counter].b = COLOUR_IDLE_BLUE / strength;
		}
	}
	for (counter = 0; counter < MAX_LED_S; ++counter) {
		if (ledLine == 3) {
			sm[counter].r = COLOUR_IDLE_RED / strength; sm[counter].g = COLOUR_IDLE_GREEN / strength; sm[counter].b = COLOUR_IDLE_BLUE / strength;
		}
	}
	if (ledNum == -99) {
		return;
	}
	if (ledLine == HOURS) {
		if (ledNum < 6) {
			ledNumChange = (ledNum + 6);
		} else {
			ledNumChange = (ledNum - 6);
		}
		h[ledNumChange].r = lightRed / strength; h[ledNumChange].g = lightGreen / strength; h[ledNumChange].b = lightBlue / strength;
	}
	if (ledLine == MINUTES) {
		if (ledNum < 6) {
			ledNumChange = (ledNum + 6);
		} else {
			ledNumChange = (ledNum - 6);
		}
		m[ledNumChange].r = lightRed / strength; m[ledNumChange].g = lightGreen / strength; m[ledNumChange].b = lightBlue / strength;
	}
	ledNumChange = ((ledNum - 3) * (-1));
	if (ledLine == SUB_MINUTES) {
		sm[ledNumChange].r = lightRed / strength; sm[ledNumChange].g = lightGreen / strength; sm[ledNumChange].b = lightBlue / strength;
	}
	return;
}

//Uses the built in functions of the i2cmaster to clear the alarm register values on the RTC
void stop_alarm() {
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_CONTROL);
	i2c_write(0b00000000);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM0_REG_RTCWKDAY);
	i2c_write(0b00000000);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM1_REG_RTCWKDAY);
	i2c_write(0b00000000);
	i2c_stop();
	return;
}

//Uses the built in functions of the i2cmaster to set the alarm register values on the RTC
void set_alarm() {
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_CONTROL);
	i2c_write(0b00000000);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM0_REG_RTCWKDAY);
	i2c_write(0b00010001);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM1_REG_RTCWKDAY);
	i2c_write(0b00100001);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM0_REG_RTCMIN);
	i2c_write(dec_to_bcd_convert(alarm[0]));
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM1_REG_RTCHOUR);
	if (alarm[2] == TIME_AM) {
		i2c_write((dec_to_bcd_convert(alarm[1])) | 0b01000000);
	} else {
		i2c_write((dec_to_bcd_convert(alarm[1])) | 0b01100000);
	}
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_CONTROL);
	i2c_write(0b00110000);
	i2c_stop();
	alarmFlag = TRUE;
	return;
}

//Uses the built in functions of the i2cmaster to set the time register values on the RTC
void set_time(int change) {
	unsigned char test;
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCWKDAY);
	i2c_rep_start(MCP7940N_R);
	test = bcd_to_dec_convert(0b00001000 & i2c_readNak());
	i2c_stop();
	if ((test == 0b00001000) && (change == FALSE)) {
		return;
	}
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCSEC);
	i2c_write(0b00000000);
	i2c_stop();
	_delay_ms(20);
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCMIN);
	i2c_write(dec_to_bcd_convert(timeStamp[1]));
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCHOUR);
	if (timeStamp[3] == TIME_AM) {
		i2c_write(((dec_to_bcd_convert(timeStamp[2])) | 0b01000000) & 0b01011111);
	} else {
		i2c_write(((dec_to_bcd_convert(timeStamp[2])) | 0b01100000) & 0b01111111);
	}
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCWKDAY);
	i2c_write(0b00001001);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCDATE);
	i2c_write(0b00000001);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCMTH);
	i2c_write(0b00000001);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCYEAR);
	i2c_write(0b00000001);
	i2c_stop();
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCSEC);
	i2c_write(0b10000000 | (dec_to_bcd_convert(timeStamp[0])));
	i2c_stop();
	return;
}

//Uses the built in functions of the i2cmaster to get the time register values on the RTC and uses them to update the timeStamp array global variable. Also checks the current minute value against the current value and if they don't match it calls the flash_weather function
void get_time() {
	unsigned char compareMin;
	unsigned char value;
	unsigned char value2;
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCMIN);
	i2c_rep_start(MCP7940N_R);
	compareMin = bcd_to_dec_convert(0b01111111 & i2c_readNak());
	i2c_stop();
	if ((compareMin != timeStamp[1]) && (weatherSaved != -1)) {
		flash_weather(weatherSaved);
	}
	timeStamp[1] = compareMin;
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCHOUR);
	i2c_rep_start(MCP7940N_R);
	value = i2c_readNak();
	value2 = value & 0b00011111;
	timeStamp[2] = bcd_to_dec_convert(value2);
	i2c_stop();
	value &= 0b00100000;
	if (value == 0b00100000) {
		timeStamp[3] = TIME_PM;
	} else {
		timeStamp[3] = TIME_AM;
	}
	return;
}

//Uses the values stored in the timeStamp array to calculate which LED's need to be activated and then sends that information to the change_leds function. Then calls the update_leds function
void update_clock() {
	if (timeStamp[2] == 12) {
		change_leds(HOURS, T1);
	} else {
		change_leds(HOURS, timeStamp[2]);
	}
	change_leds(MINUTES, (timeStamp[1] / 5));
	if (timeStamp[1] != 0) {
		if (((timeStamp[1]) % 5)  == 0) {
			change_leds(SUB_MINUTES, -99);
		} else {
			change_leds(SUB_MINUTES, ((timeStamp[1] % 5) - 1));
		}
	} else {
		change_leds(SUB_MINUTES, -99);	
	}
	update_leds();
	return;
}

//Uses the functions built into the uart library to pull the next stored character from a RX input buffer and converts this character to an int, then updates the weatherSaved global variable and send the weather int value to the flash_weather function 
void recieved_weather() {
	char weatherTypeString[2];
	int weatherValue = uartGetByte();
	weatherTypeString[0] = weatherValue;
	weatherTypeString[1] = '\n';
	weatherValue = (int) strtol(weatherTypeString, NULL, 10);
	weatherSaved = weatherValue;
	flash_weather(weatherValue);
	return;
}

//Uses the functions built into the uart library to pull the next stored character from a RX input buffer and stores them in an appropriate array. These arrays are then converted to ints and stored in the timeStamp array global variable. Then calls the set_time function 
void recieved_time() {
	int letter;
	char secTime[3];
	char minTime[3];
	char hourTime[3];
	char ampmTime[2];
	int counter;
	for (counter = 0; counter < 7; counter++) {
		letter = uartGetByte();
		if (counter == 0) {
			secTime[0] = letter;
		} else if (counter == 1) {
			secTime[1] = letter;
			secTime[2] = '\n';
		} else if (counter == 2) {
			minTime[0] = letter;
		} else if (counter == 3) {
			minTime[1] = letter;
			minTime[2] = '\n';
		} else if (counter == 4) {
			hourTime[0] = letter;
		} else if (counter == 5) {
			hourTime[1] = letter;
			hourTime[2] = '\n';
		} else if (counter == 6) {
			ampmTime[0] = letter;
			ampmTime[1] = '\n';
		}
	}
	timeStamp[0] = (unsigned char) strtol(secTime, NULL, 10);
	timeStamp[1] = (unsigned char) strtol(minTime, NULL, 10);
	timeStamp[2] = (unsigned char) strtol(hourTime, NULL, 10);
	timeStamp[3] = (unsigned char) strtol(ampmTime, NULL, 10);
	set_time(TRUE);
	return;
}

//Uses the functions built into the uart library to pull the next stored character from a RX input buffer and stores them in an appropriate array. These arrays are then converted to ints and stored in the alarm array global variable. Then calls the set_alarm function
void recieved_alarm() {
	int letter;
	char minAlm[3];
	char hourAlm[3];
	char ampmAm[2];
	int counter;
	for (counter = 0; counter < 5; counter++) {
		letter = uartGetByte();
		if (counter == 0) {
			minAlm[0] = letter;
		} else if (counter == 1) {
			minAlm[1] = letter;
			minAlm[2] = '\n';
		} else if (counter == 2) {
			hourAlm[0] = letter;
		} else if (counter == 3) {
			hourAlm[1] = letter;
			hourAlm[2] = '\n';
		} else if (counter == 4) {
			ampmAm[0] = letter;
			ampmAm[1] = '\n';
		}
	}
	alarm[0] = (unsigned char) strtol(minAlm, NULL, 10);
	alarm[1] = (unsigned char) strtol(hourAlm, NULL, 10);
	alarm[2] = (unsigned char) strtol(ampmAm, NULL, 10);
	set_alarm();
	return;
}

//Controls the animation and sounds of the alarm
void play_alarm() {
	int red;
	int green;
	int blue;
	int counter;
	int modCount;
	for (counter = 0; counter < 20; counter++) {
		modCount = counter % 2;
		brightness_control();
		if (brightness == LOW) {
			red =  COLOUR_DAY_RED / DULL;
			green = COLOUR_DAY_GREEN / DULL;
			blue = COLOUR_DAY_BLUE / DULL;
		} else {
			red =  COLOUR_DAY_RED / BRIGHT;
			green = COLOUR_DAY_GREEN / BRIGHT;
			blue = COLOUR_DAY_BLUE / BRIGHT;
		}
		if (modCount == 0) {
			h[T1].r = red; h[T1].g = green; h[T1].b = blue;
			h[T2].r = red; h[T2].g = green; h[T2].b = blue;
			h[T3].r = red; h[T3].g = green; h[T3].b = blue;
			h[T4].r = red; h[T4].g = green; h[T4].b = blue;
			h[T5].r = red; h[T5].g = green; h[T5].b = blue;
			h[T6].r = red; h[T6].g = green; h[T6].b = blue;
			h[T7].r = red; h[T7].g = green; h[T7].b = blue;
			h[T8].r = red; h[T8].g = green; h[T8].b = blue;
			h[T9].r = red; h[T9].g = green; h[T9].b = blue;
			h[T10].r = red; h[T10].g = green; h[T10].b = blue;
			h[T11].r = red; h[T11].g = green; h[T11].b = blue;
			h[T12].r = red; h[T12].g = green; h[T12].b = blue;
			m[T1].r = red; m[T1].g = green; m[T1].b = blue;
			m[T2].r = red; m[T2].g = green; m[T2].b = blue;
			m[T3].r = red; m[T3].g = green; m[T3].b = blue;
			m[T4].r = red; m[T4].g = green; m[T4].b = blue;
			m[T5].r = red; m[T5].g = green; m[T5].b = blue;
			m[T6].r = red; m[T6].g = green; m[T6].b = blue;
			m[T7].r = red; m[T7].g = green; m[T7].b = blue;
			m[T8].r = red; m[T8].g = green; m[T8].b = blue;
			m[T9].r = red; m[T9].g = green; m[T9].b = blue;
			m[T10].r = red; m[T10].g = green; m[T10].b = blue;
			m[T11].r = red; m[T11].g = green; m[T11].b = blue;
			m[T12].r = red; m[T12].g = green; m[T12].b = blue;	
			sm[T1].r = OFF; sm[T1].g = OFF; sm[T1].b = OFF;
			sm[T2].r = OFF; sm[T2].g = OFF; sm[T2].b = OFF;
			sm[T3].r = OFF; sm[T3].g = OFF; sm[T3].b = OFF;
			sm[T4].r = OFF; sm[T4].g = OFF; sm[T4].b = OFF;
			update_leds();
			play_note(2, 220);
			play_note(6, 220);
		} else {
			h[T1].r = OFF; h[T1].g = OFF; h[T1].b = OFF;
			h[T2].r = OFF; h[T2].g = OFF; h[T2].b = OFF;
			h[T3].r = OFF; h[T3].g = OFF; h[T3].b = OFF;
			h[T4].r = OFF; h[T4].g = OFF; h[T4].b = OFF;
			h[T5].r = OFF; h[T5].g = OFF; h[T5].b = OFF;
			h[T6].r = OFF; h[T6].g = OFF; h[T6].b = OFF;
			h[T7].r = OFF; h[T7].g = OFF; h[T7].b = OFF;
			h[T8].r = OFF; h[T8].g = OFF; h[T8].b = OFF;
			h[T9].r = OFF; h[T9].g = OFF; h[T9].b = OFF;
			h[T10].r = OFF; h[T10].g = OFF; h[T10].b = OFF;
			h[T11].r = OFF; h[T11].g = OFF; h[T11].b = OFF;
			h[T12].r = OFF; h[T12].g = OFF; h[T12].b = OFF;
			m[T1].r = OFF; m[T1].g = OFF; m[T1].b = OFF;
			m[T2].r = OFF; m[T2].g = OFF; m[T2].b = OFF;
			m[T3].r = OFF; m[T3].g = OFF; m[T3].b = OFF;
			m[T4].r = OFF; m[T4].g = OFF; m[T4].b = OFF;
			m[T5].r = OFF; m[T5].g = OFF; m[T5].b = OFF;
			m[T6].r = OFF; m[T6].g = OFF; m[T6].b = OFF;
			m[T7].r = OFF; m[T7].g = OFF; m[T7].b = OFF;
			m[T8].r = OFF; m[T8].g = OFF; m[T8].b = OFF;
			m[T9].r = OFF; m[T9].g = OFF; m[T9].b = OFF;
			m[T10].r = OFF; m[T10].g = OFF; m[T10].b = OFF;
			m[T11].r = OFF; m[T11].g = OFF; m[T11].b = OFF;
			m[T12].r = OFF; m[T12].g = OFF; m[T12].b = OFF;
			sm[T1].r = OFF; sm[T1].g = OFF; sm[T1].b = OFF;
			sm[T2].r = OFF; sm[T2].g = OFF; sm[T2].b = OFF;
			sm[T3].r = OFF; sm[T3].g = OFF; sm[T3].b = OFF;
			sm[T4].r = OFF; sm[T4].g = OFF; sm[T4].b = OFF;
			update_leds();
			play_note(4, 220);
			play_note(8, 220);
		}
	}
	return;
}

//Checks to see if the MFP line from the RTC is low and if the alarmFlag value is TRUE. If so then calls the play-alarm function followed by the stop_alarm function
void check_alarm() {
	if (alarmFlag == TRUE) {
		if(!(PINC & (1 << PC3))) {
			play_alarm();
			stop_alarm();
			alarmFlag = FALSE;
		}
	}
	return;
}

//Controls the animation for when the IR is transferring data
void ir_transfer() {
	int red = 255;
	int counter;
	int countMod;
	int strength;
	h[T1].r = OFF; h[T1].g = OFF; h[T1].b = OFF;
	h[T2].r = OFF; h[T2].g = OFF; h[T2].b = OFF;
	h[T3].r = OFF; h[T3].g = OFF; h[T3].b = OFF;
	h[T4].r = OFF; h[T4].g = OFF; h[T4].b = OFF;
	h[T5].r = OFF; h[T5].g = OFF; h[T5].b = OFF;
	h[T6].r = OFF; h[T6].g = OFF; h[T6].b = OFF;
	h[T7].r = OFF; h[T7].g = OFF; h[T7].b = OFF;
	h[T8].r = OFF; h[T8].g = OFF; h[T8].b = OFF;
	h[T9].r = OFF; h[T9].g = OFF; h[T9].b = OFF;
	h[T10].r = OFF; h[T10].g = OFF; h[T10].b = OFF;
	h[T11].r = OFF; h[T11].g = OFF; h[T11].b = OFF;
	h[T12].r = OFF; h[T12].g = OFF; h[T12].b = OFF;
	m[T1].r = OFF; m[T1].g = OFF; m[T1].b = OFF;
	m[T2].r = OFF; m[T2].g = OFF; m[T2].b = OFF;
	m[T3].r = OFF; m[T3].g = OFF; m[T3].b = OFF;
	m[T4].r = OFF; m[T4].g = OFF; m[T4].b = OFF;
	m[T5].r = OFF; m[T5].g = OFF; m[T5].b = OFF;
	m[T6].r = OFF; m[T6].g = OFF; m[T6].b = OFF;
	m[T7].r = OFF; m[T7].g = OFF; m[T7].b = OFF;
	m[T8].r = OFF; m[T8].g = OFF; m[T8].b = OFF;
	m[T9].r = OFF; m[T9].g = OFF; m[T9].b = OFF;
	m[T10].r = OFF; m[T10].g = OFF; m[T10].b = OFF;
	m[T11].r = OFF; m[T11].g = OFF; m[T11].b = OFF;
	m[T12].r = OFF; m[T12].g = OFF; m[T12].b = OFF;
	for (counter = 0; counter < 12; ++counter) {
		countMod = counter % 4;
		countMod = ((countMod - 3) * (-1));
		if (brightness == LOW) {
			strength = DULL;
		} else {
			strength = BRIGHT;
		}
		sm[T1].r = COLOUR_IDLE_RED / strength; sm[T1].g = COLOUR_IDLE_GREEN / strength; sm[T1].b = COLOUR_IDLE_BLUE / strength;
		sm[T2].r = COLOUR_IDLE_RED / strength; sm[T2].g = COLOUR_IDLE_GREEN / strength; sm[T2].b = COLOUR_IDLE_BLUE / strength;
		sm[T3].r = COLOUR_IDLE_RED / strength; sm[T3].g = COLOUR_IDLE_GREEN / strength; sm[T3].b = COLOUR_IDLE_BLUE / strength;
		sm[T4].r = COLOUR_IDLE_RED / strength; sm[T4].g = COLOUR_IDLE_GREEN / strength; sm[T4].b = COLOUR_IDLE_BLUE / strength;
		sm[countMod].r = red / strength; sm[countMod].g = OFF; sm[countMod].b = OFF;
		update_leds();
		_delay_ms(100);
	}
	update_clock();
	update_leds();
	return;
}

//Controls the animation and sounds for when the IR dongle is first connected to the computer running the control software
void ir_splash() {
	int red;
	int green;
	int blue;
	int counter;
	int modCount;
	h[T1].r = OFF; h[T1].g = OFF; h[T1].b = OFF;
	h[T2].r = OFF; h[T2].g = OFF; h[T2].b = OFF;
	h[T3].r = OFF; h[T3].g = OFF; h[T3].b = OFF;
	h[T4].r = OFF; h[T4].g = OFF; h[T4].b = OFF;
	h[T5].r = OFF; h[T5].g = OFF; h[T5].b = OFF;
	h[T6].r = OFF; h[T6].g = OFF; h[T6].b = OFF;
	h[T7].r = OFF; h[T7].g = OFF; h[T7].b = OFF;
	h[T8].r = OFF; h[T8].g = OFF; h[T8].b = OFF;
	h[T9].r = OFF; h[T9].g = OFF; h[T9].b = OFF;
	h[T10].r = OFF; h[T10].g = OFF; h[T10].b = OFF;
	h[T11].r = OFF; h[T11].g = OFF; h[T11].b = OFF;
	h[T12].r = OFF; h[T12].g = OFF; h[T12].b = OFF;
	m[T1].r = OFF; m[T1].g = OFF; m[T1].b = OFF;
	m[T2].r = OFF; m[T2].g = OFF; m[T2].b = OFF;
	m[T3].r = OFF; m[T3].g = OFF; m[T3].b = OFF;
	m[T4].r = OFF; m[T4].g = OFF; m[T4].b = OFF;
	m[T5].r = OFF; m[T5].g = OFF; m[T5].b = OFF;
	m[T6].r = OFF; m[T6].g = OFF; m[T6].b = OFF;
	m[T7].r = OFF; m[T7].g = OFF; m[T7].b = OFF;
	m[T8].r = OFF; m[T8].g = OFF; m[T8].b = OFF;
	m[T9].r = OFF; m[T9].g = OFF; m[T9].b = OFF;
	m[T10].r = OFF; m[T10].g = OFF; m[T10].b = OFF;
	m[T11].r = OFF; m[T11].g = OFF; m[T11].b = OFF;
	m[T12].r = OFF; m[T12].g = OFF; m[T12].b = OFF;
	sm[T1].r = OFF; sm[T1].g = OFF; sm[T1].b = OFF;
	sm[T2].r = OFF; sm[T2].g = OFF; sm[T2].b = OFF;
	sm[T3].r = OFF; sm[T3].g = OFF; sm[T3].b = OFF;
	sm[T4].r = OFF; sm[T4].g = OFF; sm[T4].b = OFF;
	for (counter = 0; counter < 7; counter++) {
		brightness_control();
		if (brightness == LOW) {
			red =  COLOUR_DAY_RED / DULL;
			green = COLOUR_DAY_GREEN / DULL;
			blue = COLOUR_DAY_BLUE / DULL;
		} else {
			red =  COLOUR_DAY_RED / BRIGHT;
			green = COLOUR_DAY_GREEN / BRIGHT;
			blue = COLOUR_DAY_BLUE / BRIGHT;
		}
		if (counter != 6) {
			h[(6 + counter)].r = red; h[(6 + counter)].g = green; h[(6 + counter)].b = blue;
		}
		h[(6 - counter)].r = red; h[(6 - counter)].g = green; h[(6 - counter)].b = blue;
		update_leds();
		play_note((counter + 2), 105);
	}
	for (counter = 0; counter < 7; counter++) {
		brightness_control();
		if (brightness == LOW) {
			red =  COLOUR_DAY_RED / DULL;
			green = COLOUR_DAY_GREEN / DULL;
			blue = COLOUR_DAY_BLUE / DULL;
		} else {
			red =  COLOUR_DAY_RED / BRIGHT;
			green = COLOUR_DAY_GREEN / BRIGHT;
			blue = COLOUR_DAY_BLUE / BRIGHT;
		}
		if (counter != 0) {
			m[(12 - counter)].r = red; m[(12 - counter)].g = green; m[(12 - counter)].b = blue;
		}
		m[counter].r = red; m[counter].g = green; m[counter].b = blue;
		update_leds();
		play_note((counter + 2), 105);
	}
	for (counter = 0; counter < 14; counter++) {
		modCount = counter % 2;
		brightness_control();
		if (brightness == LOW) {
			red =  COLOUR_DAY_RED / DULL;
			green = COLOUR_DAY_GREEN / DULL;
			blue = COLOUR_DAY_BLUE / DULL;
		} else {
			red =  COLOUR_DAY_RED / BRIGHT;
			green = COLOUR_DAY_GREEN / BRIGHT;
			blue = COLOUR_DAY_BLUE / BRIGHT;
		}
		if (modCount == 0) {
			h[T1].r = red; h[T1].g = green; h[T1].b = blue;
			h[T2].r = red; h[T2].g = green; h[T2].b = blue;
			h[T3].r = red; h[T3].g = green; h[T3].b = blue;
			h[T4].r = red; h[T4].g = green; h[T4].b = blue;
			h[T5].r = red; h[T5].g = green; h[T5].b = blue;
			h[T6].r = red; h[T6].g = green; h[T6].b = blue;
			h[T7].r = red; h[T7].g = green; h[T7].b = blue;
			h[T8].r = red; h[T8].g = green; h[T8].b = blue;
			h[T9].r = red; h[T9].g = green; h[T9].b = blue;
			h[T10].r = red; h[T10].g = green; h[T10].b = blue;
			h[T11].r = red; h[T11].g = green; h[T11].b = blue;
			h[T12].r = red; h[T12].g = green; h[T12].b = blue;
			m[T1].r = red; m[T1].g = green; m[T1].b = blue;
			m[T2].r = red; m[T2].g = green; m[T2].b = blue;
			m[T3].r = red; m[T3].g = green; m[T3].b = blue;
			m[T4].r = red; m[T4].g = green; m[T4].b = blue;
			m[T5].r = red; m[T5].g = green; m[T5].b = blue;
			m[T6].r = red; m[T6].g = green; m[T6].b = blue;
			m[T7].r = red; m[T7].g = green; m[T7].b = blue;
			m[T8].r = red; m[T8].g = green; m[T8].b = blue;
			m[T9].r = red; m[T9].g = green; m[T9].b = blue;
			m[T10].r = red; m[T10].g = green; m[T10].b = blue;
			m[T11].r = red; m[T11].g = green; m[T11].b = blue;
			m[T12].r = red; m[T12].g = green; m[T12].b = blue;
			sm[T1].r = OFF; sm[T1].g = OFF; sm[T1].b = OFF;
			sm[T2].r = OFF; sm[T2].g = OFF; sm[T2].b = OFF;
			sm[T3].r = OFF; sm[T3].g = OFF; sm[T3].b = OFF;
			sm[T4].r = OFF; sm[T4].g = OFF; sm[T4].b = OFF;
			update_leds();
			play_note(3, 100);
		} else {
			h[T1].r = OFF; h[T1].g = OFF; h[T1].b = OFF;
			h[T2].r = OFF; h[T2].g = OFF; h[T2].b = OFF;
			h[T3].r = OFF; h[T3].g = OFF; h[T3].b = OFF;
			h[T4].r = OFF; h[T4].g = OFF; h[T4].b = OFF;
			h[T5].r = OFF; h[T5].g = OFF; h[T5].b = OFF;
			h[T6].r = OFF; h[T6].g = OFF; h[T6].b = OFF;
			h[T7].r = OFF; h[T7].g = OFF; h[T7].b = OFF;
			h[T8].r = OFF; h[T8].g = OFF; h[T8].b = OFF;
			h[T9].r = OFF; h[T9].g = OFF; h[T9].b = OFF;
			h[T10].r = OFF; h[T10].g = OFF; h[T10].b = OFF;
			h[T11].r = OFF; h[T11].g = OFF; h[T11].b = OFF;
			h[T12].r = OFF; h[T12].g = OFF; h[T12].b = OFF;
			m[T1].r = OFF; m[T1].g = OFF; m[T1].b = OFF;
			m[T2].r = OFF; m[T2].g = OFF; m[T2].b = OFF;
			m[T3].r = OFF; m[T3].g = OFF; m[T3].b = OFF;
			m[T4].r = OFF; m[T4].g = OFF; m[T4].b = OFF;
			m[T5].r = OFF; m[T5].g = OFF; m[T5].b = OFF;
			m[T6].r = OFF; m[T6].g = OFF; m[T6].b = OFF;
			m[T7].r = OFF; m[T7].g = OFF; m[T7].b = OFF;
			m[T8].r = OFF; m[T8].g = OFF; m[T8].b = OFF;
			m[T9].r = OFF; m[T9].g = OFF; m[T9].b = OFF;
			m[T10].r = OFF; m[T10].g = OFF; m[T10].b = OFF;
			m[T11].r = OFF; m[T11].g = OFF; m[T11].b = OFF;
			m[T12].r = OFF; m[T12].g = OFF; m[T12].b = OFF;
			sm[T1].r = OFF; sm[T1].g = OFF; sm[T1].b = OFF;
			sm[T2].r = OFF; sm[T2].g = OFF; sm[T2].b = OFF;
			sm[T3].r = OFF; sm[T3].g = OFF; sm[T3].b = OFF;
			sm[T4].r = OFF; sm[T4].g = OFF; sm[T4].b = OFF;
			update_leds();
			play_note(6, 100);
		}
	}
	for (counter = 0; counter < 7; counter++) {
		brightness_control();
		if (brightness == LOW) {
			red =  COLOUR_DAY_RED / DULL;
			green = COLOUR_DAY_GREEN / DULL;
			blue = COLOUR_DAY_BLUE / DULL;
		} else {
			red =  COLOUR_DAY_RED / BRIGHT;
			green = COLOUR_DAY_GREEN / BRIGHT;
			blue = COLOUR_DAY_BLUE / BRIGHT;
		}
		if (counter != 6) {
			h[(6 + counter)].r = red; h[(6 + counter)].g = green; h[(6 + counter)].b = blue;
		}
		h[(6 - counter)].r = red; h[(6 - counter)].g = green; h[(6 - counter)].b = blue;
		update_leds();
		play_note((8 - counter), 105);
	}
	for (counter = 0; counter < 7; counter++) {
		brightness_control();
		if (brightness == LOW) {
			red =  COLOUR_DAY_RED / DULL;
			green = COLOUR_DAY_GREEN / DULL;
			blue = COLOUR_DAY_BLUE / DULL;
		} else {
			red =  COLOUR_DAY_RED / BRIGHT;
			green = COLOUR_DAY_GREEN / BRIGHT;
			blue = COLOUR_DAY_BLUE / BRIGHT;
		}
		if (counter != 6) {
			m[(6 + counter)].r = red; m[(6 + counter)].g = green; m[(6 + counter)].b = blue;
		}
		m[(6 - counter)].r = red; m[(6 - counter)].g = green; m[(6 - counter)].b = blue;
		update_leds();
		play_note((8 - counter), 105);
	}
	return;
}


//Sets the buzzer pin as output. Initializes the uart library and sets the transfer baud rate. Initializes the LDR using the adc_init function. Initializes the i2cmaster library, sets the MFP line of the RTC to input, checks if there is an alarm stored in the RTC and sets the alarmFlag global variable and checks if the RTC still has a stored time and if it doesn't it puts in a default time and calls the set_time function. Then moves into a while loop that continually checks the brightness levels and the time stored on the RTC which it uses to update the time shown on the LED's. Also continuously checks the RX input buffer and if it detects a matching character it starts to run the appropriate function. 
int main(void) {
	//Buzzer
	DDRD |= (1 << PD5);
	//IR
	uartInit();
	uartSetBaudRate(1200);
	int letter;
	//LDR
	adc_init();
	//RTC
	i2c_init();
	DDRC &= ~(1 << PC3);
	unsigned char batteryEnabledCheck;
	unsigned char alarmEnabledCheck;
	i2c_start_wait(MCP7940N_W);
	i2c_write(REG_RTCWKDAY);
	i2c_rep_start(MCP7940N_R);
	batteryEnabledCheck = (0b00001000 & i2c_readNak());
	i2c_stop();
	if (batteryEnabledCheck == 0b00000000) {
		timeStamp[0]  = 00;    // SECONDS
		timeStamp[1]  = 00;    // MINUTES
		timeStamp[2]  = 12;    // HOURS
		timeStamp[3]  = TIME_AM;    //AM or PM
		set_time(TRUE);
	}
	i2c_start_wait(MCP7940N_W);
	i2c_write(ALARM0_REG_RTCWKDAY);
	i2c_rep_start(MCP7940N_R);
	alarmEnabledCheck = (0b00010000 & i2c_readNak());
	i2c_stop();
	if (alarmEnabledCheck == 0b00010000) {
		alarmFlag = TRUE;
	}
	while (1) {
		brightness_control();
		check_alarm();
		get_time();
		update_clock();
		update_leds();
		_delay_ms(500);
		letter = uartGetByte();
		if (letter != -1) {
			if (letter == 'T') {
				ir_transfer();
				recieved_time();
			} else if (letter == 'A') {
				ir_transfer();
				recieved_alarm();
			} else if (letter == 'W') {
				ir_transfer();
				recieved_weather();
			} else if (letter == 'S') {
				ir_splash();
				ir_transfer();
				recieved_weather();				
			} else if (letter == 'R') {
				ir_transfer();
				recieved_time();
				stop_alarm();
			}
		}
		uartFlushReceiveBuffer();
	}
}
