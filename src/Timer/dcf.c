#include "dcf.h"

// 8-bit BCD decoder
// Requires n > 0
static uint8_t dcf_decode_bcd(uint8_t *frame, uint8_t start, uint8_t n)
{
	uint8_t val = 0;
	uint8_t b = 1;
	uint8_t *end;
	
	frame += start;
	end = frame + n;

	// Spared 50 bytes with bitwise tricks
	do
	{
		if (*frame++)
			val += b;
		if ((b <<= 1) == 16)
			b = 10;
	} while (frame < end);
	
	return val;
}

// Checks parity of `n' bits starting at `start'
static uint8_t dcf_parity(uint8_t *frame, uint8_t start, uint8_t n)
{
	uint8_t parity = 0;
	uint8_t *end;
	
	frame += start;
	end = frame + n;
	
	do
		parity ^= *frame++;
	while (frame < end);
	
	return parity;
}

// Check if frame is valid
uint8_t dcf_validate(uint8_t *frame)
{
	// Validate frame
	if (frame[0]) return 0;							// Start of minute - bit0 shall be 0
	if (!frame[20]) return 0;						// Start of time - bit20 shall be 1
	if (frame[17] == frame[18]) return 0;			// Exclusive CET / CEST
	if (dcf_parity(frame, 21, 8)) return 0;			// Minute parity
	if (dcf_parity(frame, 29, 7)) return 0;			// Hour parity
	if (dcf_parity(frame, 36, 23)) return 0;		// Date parity
	
	return 1;
}

// Convert DCF77 frame to seconds
uint24_t dcf_parse(uint8_t *frame)
{
	// Calculate HOURS
	uint8_t hours = dcf_decode_bcd(frame, 29, 6);	// 6 bits - hour
	
	// Calculate MINUTES
	uint16_t minutes = (uint16_t)hours * 60;
	minutes += dcf_decode_bcd(frame, 21, 7);		// 7 bits - minute
	
	// Calculate SECONDS
	// return (uint24_t)minutes * 60;
	// Spared some bytes by using 16 bit multiplication
	minutes *= 30;
	return (uint24_t)minutes << 1;
}