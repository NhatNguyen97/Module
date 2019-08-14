#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>
#include <string.h>
#define MAXTIMINGS	85
#define DHTPIN		7
#define DHT_MAXCOUNT 20000
#define DHT_MAXCOUNT_0 32000
#define DHT_MAXCOUNT_1 14500
#define DHT_PULSES 41
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

void set_default_priority(void) {
  struct sched_param sched;
  memset(&sched, 0, sizeof(sched));
  // Go back to default scheduler with default 0 priority.
  sched.sched_priority = 0;
  sched_setscheduler(0, SCHED_OTHER, &sched);
}

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
	digitalWrite( DHTPIN, LOW );
	delay( 20 );
	/* then pull it up for 40 microseconds */
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds(40);
	/* prepare to read the pin */
	pinMode( DHTPIN, INPUT );
	digitalWrite(DHTPIN,LOW);
	delayMicroseconds(80);
	
	for (int i=0; i < DHT_PULSES*2; i+=2)
	{
		while(0 == digitalRead(DHTPIN))
		{
			if(++pulseCounts[i] >= DHT_MAXCOUNT_0)
			{
				//set_default_priority();
				printf("Failed to h1\n");
				return -1;
			}
		
		}
		while(1 == digitalRead(DHTPIN))
		{
			if(++pulseCounts[i+1] >= DHT_MAXCOUNT_0)
			{
				pulseCounts[i+1] = DHT_MAXCOUNT_0;
				return -1;
				//set_default_priority();
				//printf("Failed to l %d c: %d\n",i+1,pulseCounts[i+1]);
				//return -1;
				
			}
		}
	}
	set_default_priority();
 	int  threshold = 0;
	for (i=2; i < DHT_PULSES*2; i+=2) {
		threshold += pulseCounts[i];
	}
	printf("threshhold: %d\n",threshold);
	threshold /= DHT_PULSES-1;
	printf("threshhold: %d\n",threshold);
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
		delay( 1000 ); /* wait 1sec to refresh */
	}

	return(0);
}
