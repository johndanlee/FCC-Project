#include <math.h>
#include "flight_tables_test.h"

void setup(void) {
  // 115200 baud rate
  Serial.begin(115200);
}



void loop() {
  Serial.println("Loop begin");
  delay(1000);

  Serial.println(calculate_range(115.5));

  delay(5000);
}
