// Instalacja biblioteki "Rtc by Makuna"

#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include <Wire.h> // must be included here so that Arduino library object file references work
#include <RtcDS1307.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8);

ThreeWire myWire(A0,A1,A2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);
RtcDS1302<ThreeWire> RtcAct(myWire);

const byte address[6] = "00001";

const char meds[6] = "1412";

void setup() {

    //Ustawienie portu szeregowego
    Serial.begin(9600);


    
    //RECIEVER**********************************************************
    Serial.begin(9600);
    radio.begin();
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_MAX);
    // radio.setDataRate(RF24_250KBPS);
    radio.startListening();

    
    //CLOCK ************************************************************
    //Wydrukowanie daty systemowej 
    Serial.print("Data systemowa: ");
    Serial.println(__DATE__);
    Serial.print("Godzina systemowa: ");
    Serial.println(__TIME__);

    Rtc.Begin();

    printDateTime(Rtc.GetDateTime()); 

    RtcDateTime dataKompilacji = RtcDateTime(__DATE__, __TIME__);
    
    printDateTime(dataKompilacji);
    Serial.println();
    RtcDateTime teraz = Rtc.GetDateTime();
    if (teraz < dataKompilacji) {
        Serial.println("Zmieniamy datę zegara na aktualną!");
        Rtc.SetDateTime(dataKompilacji);
    }
    Rtc.SetDateTime(dataKompilacji);
      if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    if (now < dataKompilacji) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(dataKompilacji);
    }
    else if (now > dataKompilacji) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == dataKompilacji) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
   pinMode(2, OUTPUT); // buzer do leków
   pinMode(9, OUTPUT); //buzzer do wiadomości
   pinMode(4, INPUT_PULLUP);
   pinMode( 12, OUTPUT);
   RtcAct.SetDateTime(Rtc.GetDateTime());
}

//----------------------------------------------------------------------------------------------------------------------------
int n = 0;

void loop() {
//&& Rtc.GetDateTime() == RtcAct.GetDateTime()
    
    if(check_date(Rtc.GetDateTime(), meds) == true  && n != 1)
    {
      //RtcAct.SetDateTime(Rtc.GetDateTime());
     // RtcAct.SetIsRunning(false);
      n = 2;
      Serial.println("wez leki");
      //digitalWrite(12, HIGH);
      digitalWrite(2, HIGH);      
    }
    if(digitalRead(4)== LOW && n == 2)
    {
      //digitalWrite(12, LOW);
      digitalWrite(2, LOW);
      n=1;
      /*Rtc.SetIsRunning(true);
      RtcAct.SetDateTime(Rtc.GetDateTime());*/
    }
    if (check_date(Rtc.GetDateTime(), meds) == false)
      n=0;

    //odbiornik
    if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    
    digitalWrite(9, HIGH);
    delay(150);
    digitalWrite(9, LOW);
       
    }
    else
    {
      digitalWrite(9, LOW);
    }
    //printDateTime(Rtc.GetDateTime());
    //delay(1000);
}

//------------------------------------------------------------------------------------------------------------------------------
#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) {
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(), dt.Day(), dt.Year(),
            dt.Hour(), dt.Minute(), dt.Second() );
    Serial.println(datestring);
}

boolean check_date(const RtcDateTime& dt, const char* meds)
{
    char datestring[6];
    snprintf_P(datestring,countof(datestring),PSTR("%02u%02u"),dt.Hour(), dt.Minute());
    int n=3;
    boolean czy = true;
    while(n>=1 && czy == true)
    {
      //Serial.println(datestring[n]);
      //Serial.println(meds[n]);
      if((datestring[n] != meds[n]))
        czy = false;
      n--;
    }
    return(czy);
}
