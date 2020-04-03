/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#include <stdio.h>
#include <string.h>
#include "rn4871_click.h"
#include "driver_rn4871.h"
#include "usart.h"
#include "clock_config.h"
#include "util/delay.h"

typedef enum { NO_RESPONSE, OK } rn4871_Responses_t;

volatile uint16_t rn_index = 0;
volatile uint8_t  rn_buffer[RN_BUFF_SIZE];

/**************************************RN4871_CaptureReceivedMessage*******************************
    Read received data from BLE module
**************************************************************************************************/
void RN4871_CaptureReceivedMessage()
{
	uint8_t data = usart_ble_get_c();
	if(rn_index < RN_BUFF_SIZE)
	{
		rn_buffer[rn_index++] = data;
	}
}

/**************************************RN4871_Setup_Transparent_UART_service***********************
     Configure RN4871/70 in UART Transparent service
     Setup Bluetooth module name
**************************************************************************************************/
void RN4871_Setup_Transparent_UART_service()
{
	RN4871_Reset_Module();
	RN4871_sendAndWait("SF,1\r", "", 10); /* Factory Reset */
	RN4871_sendAndWait("$$$", "CMD> ", 10);
	RN4871_sendAndWait("SN,tiny2AVR-DEMO\r", "AOK\r\nCMD> ", 10); /* Configures module name as 'tiny2AVR-DEMO' */
	RN4871_sendAndWait("SS,C0\r", "AOK\r\nCMD> ", 10);
	RN4871_sendAndWait("R,1\r", "Rebooting", 10);
	RN4871_ClearReceivedMessage();
}

/**************************************RN4871_Reset_Module*****************************************
    Reset BLE module
**************************************************************************************************/
void RN4871_Reset_Module(void)
{
	rn4871_ClearResetPin(); /* Reset using GPIO */
	RN4871_blockingWait(1);
	rn4871_SetResetPin();
	RN4871_blockingWait(50);
}

/**************************************RN4871_ClearReceivedMessage*********************************
    Clear BLE receive buffer
**************************************************************************************************/
void RN4871_ClearReceivedMessage(void)
{
	memset((void *)rn_buffer, 0, RN_BUFF_SIZE);
	rn_index = 0;
}

/**************************************RN4871_CheckResponse****************************************
    Check the response
**************************************************************************************************/
uint8_t RN4871_CheckResponse(const char *response)
{
	uint8_t ret = 0;
	if(strstr((const char *)rn_buffer, response))
	{
		ret = 1;
	}
	return ret;
}

/**************************************RN4871_blockingWait*****************************************
    Busy-waiting delay
**************************************************************************************************/
void RN4871_blockingWait(uint16_t limit)
{
	for(uint16_t counter = 0; counter < limit; counter++)
	{
		_delay_ms(15);
	}
}

/**************************************RN4871_sendAndWait******************************************
    Send string and wait for response
**************************************************************************************************/
void RN4871_sendAndWait(const char *sendString, const char *response, uint16_t delay)
{
	do
	{
		RN4871_ClearReceivedMessage();
		rn4871_SendString(sendString);
		RN4871_blockingWait(delay);
	}
	while(RN4871_CheckResponse(response) == NO_RESPONSE);

}
