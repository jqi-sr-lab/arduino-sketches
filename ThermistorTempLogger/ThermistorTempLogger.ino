#include "SPI.h"

struct _s  { const char * n; int val;};
typedef struct _s my_s_t;

my_s_t t = {.n = "name", .val = 42}; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(t.n);
  Serial.println(t.val);
  delay(1000);

}
