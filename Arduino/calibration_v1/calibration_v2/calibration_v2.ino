
// Basic demo for accelerometer readings from Adafruit LIS3DH

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();



// include the library code:
#include "Adafruit_LiquidCrystal.h"

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);


// for moving average
int count = 0;
int xarr[] = {0,0,0,0,0};
int yarr[] = {0,0,0,0,0};
int zarr[] = {0,0,0,0,0};



void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("LIS3DH test!");

  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  // lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // lis.setDataRate(LIS3DH_DATARATE_50_HZ);
  Serial.print("Data rate set to: ");
  switch (lis.getDataRate()) {
    case LIS3DH_DATARATE_1_HZ: Serial.println("1 Hz"); break;
    case LIS3DH_DATARATE_10_HZ: Serial.println("10 Hz"); break;
    case LIS3DH_DATARATE_25_HZ: Serial.println("25 Hz"); break;
    case LIS3DH_DATARATE_50_HZ: Serial.println("50 Hz"); break;
    case LIS3DH_DATARATE_100_HZ: Serial.println("100 Hz"); break;
    case LIS3DH_DATARATE_200_HZ: Serial.println("200 Hz"); break;
    case LIS3DH_DATARATE_400_HZ: Serial.println("400 Hz"); break;

    case LIS3DH_DATARATE_POWERDOWN: Serial.println("Powered Down"); break;
    case LIS3DH_DATARATE_LOWPOWER_5KHZ: Serial.println("5 Khz Low Power"); break;
    case LIS3DH_DATARATE_LOWPOWER_1K6HZ: Serial.println("16 Khz Low Power"); break;
  }







  // set up the LCD's number of rows and columns:
  if (!lcd.begin(16, 2)) {
    Serial.println("Could not init backpack. Check wiring.");
    while(1);
  }
  Serial.println("Backpack init'd.");



  lcd.setCursor(0, 0);
  lcd.print("Acceleration");






}

void loop() {
  lis.read();      // get X Y and Z data

    // // Check arrays
    // Serial.print("Before copy: "); 
    // for(int i = 0; i < 5; i++){Serial.print(xarr[i]); Serial.print(" ");}
    // Serial.print("\t");
    // for(int i = 0; i < 5; i++){Serial.print(yarr[i]); Serial.print(" ");}
    // Serial.print("\t");
    // for(int i = 0; i < 5; i++){Serial.print(zarr[i]); Serial.print(" ");}
    // Serial.println();


  int i;
  for(i = 0; i <= 3; i++){
    xarr[i] = xarr[i + 1];
    yarr[i] = yarr[i + 1];
    zarr[i] = zarr[i + 1];
  }


  xarr[4] = lis.x;
  yarr[4] = lis.y;
  zarr[4] = lis.z;


  long sumx = 0;
  long sumy = 0;
  long sumz = 0;
  for(i = 0; i < 5; i++){
    sumx += xarr[i];
    sumy += yarr[i];
    sumz += zarr[i];
    Serial.println(sumz);
  }
  Serial.println();

  long x = sumx/5;
  long y = sumy/5;
  long z = sumz/5;
  
    Serial.print("X:  "); Serial.print(x);
    Serial.print("  \tY:  "); Serial.print(y);
    Serial.print("  \tZ:  "); Serial.print(x);
    Serial.println();


  lcd.setCursor(15, 0);
  lcd.print(count);
  count++;
  if (count == 5){
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(x/10);  // divide by 10 to truncate the least significant digit
    lcd.setCursor(5, 1);
    lcd.print(y/10);
    lcd.setCursor(10, 1);
    lcd.print(z/10);

    count = 0;
  }


  delay(100);
}
