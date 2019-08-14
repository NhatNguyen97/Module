#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define MAXTIMINGS	85
#define DHTPIN		7
#define DHT_MAXCOUNT 32000
#define DHT_PULSES 41
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

void read_dht11_dat()
{
	int  laststate	= HIGH;
	int count	= 0;
	int j		= 0, i;
	float	f; /* fahrenheit */
	int pulseCounts[DHT_PULSES*2] = {0}; 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	/* pull pin down for 18 milliseconds */
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, HIGH );
	delay( 500 );
	/* then pull it up for 40 microseconds */
	digitalWrite( DHTPIN, LOW );
	delay( 20 );
	/* prepare to read the pin */
	pinMode( DHTPIN, INPUT );
	delayMicroseconds( 50 );
	/* detect change and read data */
	/* for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
		printf("%d",laststate);
		//if ( counter == 255 )
		//	break;

		// ignore first 3 transitions 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			// shove each bit into the storage bytes 
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 )
				dht11_dat[j / 8] |= 1;
			j++;
		}
	 } */
	laststate = digitalRead(DHTPIN);
	printf("last state: %d\n",laststate);	 
	while( 1 == digitalRead(DHTPIN))
	{
		if(++count >= DHT_MAXCOUNT)
		{
			printf("Failed to low 1\n");
		}
		
	}
	for (int i=0; i < DHT_PULSES*2; i+=2)
	{
		while(0 == digitalRead(DHTPIN))
		{
			if(++pulseCounts[i] >= DHT_MAXCOUNT)
			{
				printf("Failed to high %d\n",i);
				break;
			}
		}	
		while(1 == digitalRead(DHTPIN))
		{
			if(++pulseCounts[i+1] >= DHT_MAXCOUNT)
			{
				printf("Failed to high %d\n");
				break;
			}
		}	
	}
 	int  threshold = 0;
	for (i=2; i < DHT_PULSES*2; i+=2) {
		threshold += pulseCounts[i];
	}

	threshold /= DHT_PULSES-1;
	for (i=3; i < DHT_PULSES*2; i+=2) {
		int index = (i-3)/16;
		dht11_dat[index] <<= 1;
		if (pulseCounts[i] >= threshold) {
			dht11_dat[index] |= 1;
		}
	}
	
	/*
	 * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
	 * print it out if data is good
	 */
	if ( 
	     dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) )
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		printf( "Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",
			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
	}else  {
		printf( "Data not good, skip\n" );
	}
}

int main( void )
{
	printf( "Raspberry Pi wiringPi DHT11 Temperature test program\n" );

	if ( wiringPiSetup() == -1 )
		exit( 1 );

	while ( 1 )
	{
		read_dht11_dat();
		delay( 2000 ); /* wait 1sec to refresh */
	}

	return(0);
}
