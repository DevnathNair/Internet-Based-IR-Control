#include<Ethernet.h>
#include<SPI.h>
#include<IRremote.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Physical MAC Adress of the Ethernet Sheild.
byte ip[] = { 192, 168, 100, 10 }; // Static IP.

//RAW IR Codes.
unsigned int ACOFFraw[60]={8550,4050,450,1550,450,550,500,500,500,500,500,1500,500,500,500,500,500,500,450,1550,500,1500,450,600,400,550,450,550,500,500,500,500,500,500,500,500,500,550,450,550,450,550,450,550,450,1550,450,550,450,1550,450,550,450,550,450,550,450,1550,450,};
unsigned int ACONraw[60]{8550,4000,500,1500,550,450,550,500,450,550,500,1500,450,550,500,500,500,500,500,500,450,550,500,500,450,550,450,550,450,550,450,550,500,500,450,550,500,1500,500,500,500,500,500,500,500,500,500,500,450,550,500,500,450,1550,500,500,500,500,550,};

EthernetClient client;
EthernetServer server(80);
IRsend irsend;

class Obj
{
 private:
  static unsigned int no;
  unsigned int oid;
  unsigned int* raw;
  char* nme;

 public:
  Obj(char *x,unsigned int* r);
  void button();
  void ir();
  void check(String s);
  ~Obj();
};

unsigned int Obj::no=1;

Obj::Obj(char* x, unsigned int* r)
{
 nme =x;
 no++;
 oid=no;
 raw=r;
}

void Obj::button()
{
 client.print("<input type=button value=\"");
 client.print(nme);
 client.print(" \"onmousedown=location.href='/?");
 client.print(String(oid));
 client.println("'>&nbsp");
}

void Obj::ir()
{
 for (int i = 0; i < 3; i++)
  irsend.sendRaw(raw,60,38);
}

void Obj::check(String s)
{
  if(s.indexOf(String(oid))>0)
   ir();  
}

Obj::~Obj()
{
 no--;
}

String readString; 

void setup(){

  Ethernet.begin(mac, ip);
  server.begin();
}

void loop(){
 
  client = server.available();
  Obj AC_on("On",ACONraw); 
  Obj AC_off("Off",ACOFFraw);
  if (client) 
  {
   while (client.connected()) 
    {
      if (client.available()) 
       {
        char c = client.read();

        if (readString.length() < 100) 
          readString += c;  

        //if HTTP request has ended
        if (c == '\n') {

       
          Serial.println(readString); //Print to Serial Monitor for debugging.

          client.println("HTTP/1.1 200 OK"); //Sends a New Page
          client.println("Content-Type: text/html");
          client.println();

          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Appliance</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<center><H1>Appliance Wasabi</H1>");

          AC_on.button();
 	        AC_off.button();
           
          client.println("</BODY>");
          client.println("</HTML>");

          delay(1);
          //stopping client
          client.stop();


          AC_on.check(readString);
          AC_off.check(readString);
         
          readString="";

        }
      }
    }
  }
}


