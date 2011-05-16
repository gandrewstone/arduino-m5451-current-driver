#if defined(__AVR_ATmega328P__)  // If its not the 328, its not a Lightuino so I don't need spi stuff since I am not using my USB...

#include "WProgram.h"
#include "lightuinoUsb.h"
#include "spi.h"
#include "avr/pgmspace.h"

char LightuinoUSB::available(void)
{
  if (spiRcv.fend == spiRcv.start) xfer(0xff);
  return (spiRcv.fend != spiRcv.start);
}

int LightuinoUSB::peek(void)
{
  return fifoPeek(&spiRcv);
}

void LightuinoUSB::flush(void)
{
  // Nothing to do because we automatically flush
}

int LightuinoUSB::read(void)
{
  return fifoPop(&spiRcv);
}

int LightuinoUSB::readwait(void)
{
  while (!available()) ;  // Wait for a character
  return fifoPop(&spiRcv);
}


void LightuinoUSB::end()
  {
  disable_spi();
  }

void LightuinoUSB::begin()
  {
    // set the slaveSelectPin as an output:
    pinMode (slaveSelectPin, OUTPUT);
    digitalWrite(slaveSelectPin,LOW);
    fifoCtor(&spiRcv);
    setup_spi(SPI_MODE_0, SPI_MSB, SPI_NO_INTERRUPT, SPI_MSTR_CLK8);
  }

void LightuinoUSB::print(unsigned long int num,char base)
{
  char buf[(sizeof(unsigned long int) * 8)+1];
  unsigned char i=(sizeof(unsigned long int) * 8);

  buf[i] = 0;
  do
    {
      unsigned char temp = (num % base);
      i--;
      buf[i] = (temp<10) ? temp+'0': temp+'A'-10;
      num /= base;
    } while(num);
  print(&buf[i]);
}


void LightuinoUSB::print(char* str)
{
  while(*str!=0)
    {
      xfer(*str);
      // delayMicroseconds(200); delay moved into xfer
      str++;
    }
}

void LightuinoUSB::println(char* str)
{
  print(str);
  xfer('\n');
}

void LightuinoUSB::print(const char* str)
{
  while(*str!=0)
    {
      xfer(*str);
      // delayMicroseconds(200); delay moved into xfer
      str++;
    }
}


void LightuinoUSB::println(const char* str)
{
  print(str);
  xfer('\n');
}

void LightuinoUSB::pgm_print(const char* str)
{
  char c;
  while((c = pgm_read_byte(str))!=0)
    {
      xfer(c);
      // delayMicroseconds(200); delay moved into xfer
      str++;
    }
}

void LightuinoUSB::pgm_println(const char* str)
{
  pgm_print(str);
  xfer('\n');
}


void LightuinoUSB::xfer(char s)
{
  if (s != 0xFF)
    {
      // Empirically, 500us/character avoids overrun issues.
      while (micros() - lastMicros < 500)
        delayMicroseconds(5);
      lastMicros = micros();
    }
  uint8_t in = send_spi(s);
  if (in != 0xFF) 
    {
      fifoPush(&spiRcv, in);
      //Serial.print("spi rcvd");
      //Serial.println((int) in);
    }
}


LightuinoUSB Usb;

#endif
