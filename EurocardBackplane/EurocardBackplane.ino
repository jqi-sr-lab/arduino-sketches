/*
   EurocardBackplane.ino - Teensyduino sketch for Eurocard Backplane MCU

   Created by Neal Pisenti, 2015
   JQI - Joint Quantum Institute

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


//#include "JQI_Ethernet.h"
/*
  Web Server

 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 */
/*
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1,177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

#define WIZNET_PWDN 7
#define WIZNET_INTERRUPT 10
#define W5200_RESET_PIN 9
#define WIZNET_SS 8

void setup() {
  pinMode(WIZNET_PWDN, OUTPUT);
  digitalWrite(WIZNET_PWDN, LOW);
  //pinMode(WIZNET_INTERRUPT, INPUT);
  // as suggested...
  pinMode(WIZNET_SS, INPUT_PULLUP);
  delay(100);
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

Serial.println("first in setup");
//delay(2000);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
	  client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
*/
/*
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,1, 177);
IPAddress gateway(192,168,1, 1);
IPAddress subnet(255, 255, 255, 0);


// telnet defaults to port 23
EthernetServer server(23);
boolean alreadyConnected = false; // whether or not the client was connected previously

void setup() {
  // initialize the ethernet device
  Ethernet.begin(mac, ip, gateway, subnet);
  // start listening for clients
  server.begin();
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Chat server address:");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // wait for a new client:
  EthernetClient client = server.available();

  // when the client sends the first byte, say hello:
  if (client) {
    if (!alreadyConnected) {
      // clead out the input buffer:
      client.flush();
      Serial.println("We have a new client");
      client.println("Hello, client!");
      alreadyConnected = true;
    }

    if (client.available() > 0) {
      // read the bytes incoming from the client:
      char thisChar = client.read();
      // echo the bytes back to the client:
      server.write(thisChar);
      // echo the bytes to the server as well:
      Serial.write(thisChar);
    }
  }
}
*/

#include <SPI.h>
#include <Ethernet.h>


#include "ArduinoJson.h"
#include "ArduinoRPC.h"

#define PORT 23

// spoofed mac address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00 };
// ip address
IPAddress ip(192, 168, 1, 177);

// Etherent server
EthernetServer server(PORT);

//ArduinoRPC  rpcServ(Serial, "test descrption");
//EthernetArduinoRPC rpc(server, "test server");
//ArduinoRPC rpc("test");


/*
void meth1(){
  Serial.println("Meth1");
}

void meth2(){
  Serial.println("meth2");
}
Callback methods[] = {meth1, meth2};
//RPCTarget tt = { .name = "testtarg", .docstring ="mydocstring", .methods = methods};
*/




/*

typedef struct RPCMethod {
  const char * name;
  Callback callback;
} RPCMethod;


struct RPCThing {



  void ta(){
    Serial.println("here tt");
  }
  void tb(int var){
    Serial.print("Here is a number: ");
    Serial.println(var);
  }

  const char * name = "targ";
  const char * docstring = "targ doc";
//  RPCMethod methods[] = {{.name = "a", .callback = tt}, {.name = "b", .callback =tb}};
} tt;







//template <typename T> using CB = void * (T &);



class RPCTarget {
public:
  template <class C, typename T> struct RPCMethod {
     void (C::*method)(JsonObject &, T *);
     const char * name;
   };
  //typedef void (RPCTarget::*RPCMethod_d)(JsonObject &, double *);
//typedef void (RPCTarget::*RPCMethod_i)(JsonObject &, int *);

  RPCTarget(const char * docstring) :
    _docstring(docstring) {}

  //! Add Docstring to response.
  void docstring(JsonObject & resp){
    resp["docstring"] = _docstring;
  }




private:
  //! Description of the remote target
  const char * _docstring;


};

//typedef void (*genptr)(void);

class myRPClass : public RPCTarget {
public:
  myRPClass(const char * description) : RPCTarget(description){
    //  other init stuff can go here
      _int = 5;
      _cb.method = &myRPClass::tb2;
      _cb.name = "asdf";
      //_cb = cb;
  }
  void doThing(){
    Serial.print("Doing a thing: ");
    Serial.println(_int);
  }
void tb(int  var){
  Serial.println("here in the place");
  Serial.println(var);
}
void tb2(JsonObject & resp, int * params){
  Serial.print("resonable ");
  Serial.println(params[0]);
  Serial.println("exiting");

}
*/

/*void call(){
  Serial.println("calling from call");
  int params[] = {1,2};
  (this->*_cb)();
}*/
/*
private:
  int _int;

  RPCMethod<myRPClass, int> _cb;
  //void * _cb;
  //void (myRPClass::*_cb)(int);
  //CB<int> _cb;
  //CB _cb;
};
*/







class DAC {
public:
  // must implement this...?
  enum ExposedMethods {
    SET_VOLTAGE = 0,
    READ_VOLTAGE,
    METHOD_COUNT  // always last item in enum, cheeky way to get methoud count.
  };
typedef void (DAC::*RPCMethod)(JsonObject &, JsonObject &);
  //using RPCMethod = int (DAC::*)(JsonObject & , JsonObject &);

  DAC(const char * description, int channel) :
  //  RPCTarget(description, METHOD_COUNT),
    _value(0),
    _channel(channel)
  {
    //  other init stuff can go here
    // must add each method; perhaps an automated way to do this?
    methods[SET_VOLTAGE] = &DAC::setVoltage;
    methodName[SET_VOLTAGE] = "set-voltage";

    methods[READ_VOLTAGE] = &DAC::readVoltage;
    methodName[READ_VOLTAGE] = "read-voltage";


  }

  // here you define your methods. Returns in int status code; 0 = ok.
  void setVoltage(JsonObject & payload, JsonObject & resp){
    //StaticJsonBuffer<JSON_BUFFER_SIZE> _tmpBuf;
    //JsonObject & kwargs = _tmpBuf.parseObject(payload["kwargs"]);
    //if (! kwargs.containsKey("value")){
    if (! payload["kwargs"]["value"]){
      resp["message"] = "must pass value";
      resp["status"] = "failed";

    } else  if(payload["kwargs"]["value"] > 10){
       resp["message"] = "value out of range";
       resp["status"] = "failed";

     } else {
       _value = payload["kwargs"]["value"];

     }

  }

  // here's an example of modifying the response, eg, for a return value.
  void readVoltage(JsonObject & payload, JsonObject & resp){
    JsonObject & ret = resp["ret"];
    ret["voltage"] = _value;
    ret["channel"] = _channel;
      }

  // this must be public, and called "methods"
  //RPCMethod<DAC> methods[METHOD_COUNT];
  RPCMethod methods[METHOD_COUNT];
  const char * methodName[METHOD_COUNT];
  const char * docstring = "Docstring for this target...";
private:
  int _value, _channel;
};



//typedef void (myRPClass::*RPCMethod)(JsonObject &, JsonObject &);
//template <class T> using RPCMethod = void (T::*)(JsonObject & , JsonObject &);

DAC myDAC("DAC 0",1);
DAC myDAC2("DAC 1",2);
DAC myDAC3("DAC 2",3);

ArduinoRPC rpcServer("a server");

typedef void (*fptr)(JsonObject &, JsonObject &);

enum MemberFunc {AFUNC, BFUNC  };

void setup(){
  Ethernet.begin(mac, ip);
  server.begin();


  Serial.begin(9600);

  delay(1000);
/*
    JsonObject & payloadObj = jsonResponse.createObject();
    JsonObject & responseObj = jsonPayload.createObject();
    //RPCMethod ref;
    //void (myRPClass::*ref)(JsonObject & , JsonObject &) = &myRPClass::cb;
    payloadObj["thing"] = "asdf";
    payloadObj["asdf"] = "the really deep obj";

    payloadObj["BFUNC"] = BFUNC;
    const char * key = payloadObj["thing"];
    const char * tmp = payloadObj[key];
    Serial.println("here is: ");
    Serial.println(tmp);
    JsonArray & meth = payloadObj.createNestedArray("meth");
    meth.add(5.234,3);
    meth.add(3);
  //  (myRPClass::cb)
  //RPCMethod<myRPClass> myMeth = myRPClass::cb;
  //RPCMethod myMeth = reinterpret_cast<RPCMethod>(&myRPClass::cb);
  //void * ptr = &myRPClass::cb;
  //  fptr myMeth = reinterpret_cast<fptr> (&myRPClass::cb);
  //  payloadObj["pointer"] = (size_t)(myMeth);

    payloadObj.printTo(Serial);
    //RPCMethod<myRPClass> mm = *(void )(payloadObj["pointer"].as<size_t>());
    Serial.println("here's teh dereferenced pointer");
    //RPCMethod mm = reinterpret_cast<RPCMethod>(reinterpret_cast<fptr>(payloadObj["pointer"].as<size_t>()));
    //(myRPC->*mm)(payloadObj, responseObj);
    //(*(void *)(payloadObj["pointer"].as<size_t>()))(payloadObj, responseObj);
  //  (myRPC.*(static_cast<RPCMethod>(payloadObj["pointer"].as<size_t>())))(payloadObj, responseObj);
    //Serial.println(outp);
    //char * myt;
  //(payloadObj["pointer"].as<size_t>())(payloadObj, responseObj);
  //  Serial.println(myt);
//Serial.write(& RPCThing::ta);
  //rootObj["tt"] = &tt.ta;

//(*tt.methods[0])();
//(*tt.methods[1])();

//rpc.registerTarget("target1", "doc target 1");//, [meth1, meth2]);
  Serial.println("alive");
//rootObj.printTo(Serial);
*/
rpcServer.registerTarget(myDAC, "DAC1");
rpcServer.registerTarget(myDAC2, "DAC2");
rpcServer.registerTarget(myDAC3, "DAC3");

//Serial.println("now i still am");
//myDAC.docstring(payloadObj);
Serial.println("now it comes:");
//(myDAC.*myDAC.methods[0])(payloadObj, responseObj);
//Serial.println("done");
//payloadObj.printTo(Serial);
  //rpcServ.prepareServer(RPC_SERIAL, RPC_SERIAL);

}

void loop(){


  EthernetClient client = server.available();
  //Serial.println("yeah");
  if (client){
    Serial.println("client");
    rpcServer.processRequest(client);
    //client.stop();
  }

  // stop client after processBuffer?
  // although bad, since don't know if of type Serial or not.
  //  client.stop();

  //rpcServer.processRequest(Serial);

// serial implementation...
//rpc.processRequest(Serial);

}
