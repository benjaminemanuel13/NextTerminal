//As this has to use the classic library at the moment to get ANSI it can only be a program:

//zcc +zx term.c -create-app -lesxdos -pragma-need=ansiterminal -Cl-v -pragma-define:ansicolumns=64

#include <stdio.h>
//#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <string.h>
#include <errno.h>
#include <spectrum.h>

#define NOS_Initialise	 0x80
#define NOS_Shutdown	 0x81
#define NOS_Open	 0xF9
#define NOS_Close	 0xFA
#define NOS_OutputChar	 0xFB
#define NOS_InputChar	 0xFC
#define NOS_GetCurStrPtr 0xFD
#define NOS_SetCurStrPtr 0xFE
#define NOS_GetStrExtent 0xFF
#define NEAT_Deprecated  0x01
#define NEAT_SetCurChan	 0x02
#define NEAT_GetChanVals 0x03
#define NEAT_SetChanVals 0x04
#define NEAT_SetTimeouts 0x05
#define NEAT_AddBank	 0x06
#define NEAT_RemoveBank	 0x07
#define NEAT_GetESPLink	 0x08
#define NEAT_SetBaudRate 0x09
#define NEAT_SetBuffMode 0x0A
#define NEAT_ProcessCMD	 0x0B


static struct esx_drvapi rtc ;// = { 0,0,0 }; //Can't initialise this, if you leave it in it causes error
static struct esx_drvapi net ;// = { 'N'*256 + 0, 0, 0 };
static struct esx_drvapi send ;// = { 'N'*256 + 0, 0, 0 };

static char CONNECTstring[32] = "TCP,192.168.0.10,9000";

int main(int argc, char **argv)
{
   int x, t;
   int nethandle;

//Pointless doing a static initialiation as union in struct means it is overwritten
   rtc.call.driver = 0;	// This is the main system so RTC
   rtc.call.function = 0;	// No API for rtc
   rtc.de = 0;  // if needed
   rtc.hl = 0; // if needed

   net.call.driver = 'N';	// This is Network should be initialised above
   net.call.function = NOS_Initialise;	// Default is Initialise?
   net.de = 0;  // if needed
   net.hl = 0; // if needed

//   printf("WARNING: ANSI library not working.\n");
//   printf("%c2J", 155);
//   printf("Z88DK Classic Lib ANSI Terminal\n");

//   printf("%c[1musing ESPAT.DRV for ZX Next ", 27);
//      printf("%c[2mBy Tim Gilberts\n", 27);
//   printf("%c[4m(c) 2018 Infinite Imaginations\n", 27);
//   printf("%c[24mThanks to AA for the patience!\n", 27);
//   printf("%c[7mPress SS and BREAK to exit. ", 27);
//   printf("%c[27mH for Help text\n", 27);
   
   printf("Connecting to '%s'.\n", CONNECTstring);
   
   for (t = 1; t < 4; t++) { 
      net.call.driver = 'N';
      net.call.function = NOS_Open ;
      net.hl = CONNECTstring ;
      net.de = strlen( CONNECTstring );

//      printf("%c, %x, %u, %u\n", *((unsigned char *)net),*((unsigned char *)net + 1), *(((int *)net) +1 ), *(((int *)net) + 2));
      printf("HL is at %u of length %u.\n",(char *)CONNECTstring, strlen(CONNECTstring) );

      printf("Try %u to '%s'.\n", t, (char *)net.hl );   

      if(esx_m_drvapi(&net)) {
         printf ("NET Open Driver error %u.\n",errno);
      } else {
 
         nethandle = net.call.function; // As C is a copy of A on exit - added to API

         t = 4;
         break;
      }
   }
int running = 1;

char command[30];
for(;;)
{
   printf("Enter command: ");
   gets(command);

   int len = strlen(command);

   int lcpy = len;
   int actlen = len;

   char lenstr [4];
   sprintf(lenstr, "%04d", lcpy);
   printf("Length :");
   printf(lenstr);
   printf("\n");

   //This Loop is woring fine on second loop

   for(int i = 0; i < 4; i++)
   {
      send.call.driver = 'N';
      send.call.function = NOS_OutputChar;
      send.hl = 5;
      send.de = lenstr[i];

      if (esx_m_drvapi(&send))
         printf ("RTC Send error %u.\n",errno);
   }

   //This Loop fails on second loop

   for(int i = 0; i < len; i++)
   {
      send.call.driver = 'N';
      send.call.function = NOS_OutputChar;
      send.hl = len;

      char co = command[i];
      send.de = co;

      if (esx_m_drvapi(&send))
         printf ("RTC Send error %u.\n",errno);
   }

   char retlenst[6];

   printf("\nIncoming\n");
   for(int i = 0; i < 6; i++)
   {
      send.call.driver = 'N';
      send.call.function = NOS_InputChar;
      send.hl = 1;
      send.de = 0;

      if (esx_m_drvapi(&send))
         printf ("RTC Receive error %u.\n",errno);

      retlenst[i] = send.de;
      char tmp = send.de;

      printf("%c", tmp);
      printf(" %u ", tmp);
   }

   printf("\n\n");

   int one = (int)retlenst[0] - 48;
   int two = (int)retlenst[1] - 48;
   int three = (int)retlenst[2] - 48;
   int four = (int)retlenst[3] - 48;
   int five = (int)retlenst[4] - 48;
   int six = (int)retlenst[5] - 48;

   int retlen = 0;

   retlen += one * 100000;
   retlen += two * 10000;
   retlen += three * 1000;
   retlen += four * 100;
   retlen += five * 10;
   retlen += six;

   int retres = retlen;

   printf("Sent Length: ");
   printf("%u", retres);
   printf("\n");

   char output[100];

   for(int i = 0; i < retlen; i++)
   {
      output[i] = ' ';
   }

   for(int i = 0; i < retlen; i++)
   {
      send.call.driver = 'N';
      send.call.function = NOS_InputChar;
      send.hl = 1;
      send.de = 0;

      if (esx_m_drvapi(&send))
         printf ("RTC Receive error %u.\n",errno);

      output[i] = send.de;
      char tmp = send.de;

      printf("%c", tmp);
   }

   printf("\n\n");
}

   rtc.call.driver = 0;	// This is the main system so RTC
   rtc.call.function = 0;	// No API for rtc       
   if (esx_m_drvapi(&rtc))
      printf ("RTC Driver error %u.\n",errno);

   printf("Time BC= %u DE= %u\n\n",rtc.bc, rtc.de);

   printf("%c[m", 27);

   net.call.function = NOS_Close ;
   net.de = nethandle << 8;
   if (esx_m_drvapi(&net))
      printf ("NET Close Driver error.\n");

   printf("Press enter to continue...");
   char dummy[5];
   gets(dummy);

   return 1;
}


