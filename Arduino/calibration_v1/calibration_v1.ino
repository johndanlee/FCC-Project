
#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Math.h>

// Set up for I2C on accelerometer
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

// include the library for the LCD
#include "Adafruit_LiquidCrystal.h"
// Connect to LCD via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);


// for moving average later
int count = 0;
int xarr[15] = {0};
int yarr[15] = {0};
int zarr[15] = {0};

// for drawing degree symbol on LCD later
uint8_t degree[8] = {0xc,0x12,0x12,0xc,0x0,0x0,0x0};



void setup(void) {
  // 115200 baud rate
  Serial.begin(115200);

  // pause until serial console opens
  while (!Serial) delay(10);

  // change this to 0x19 for alternative i2c address
  if (! lis.begin(0x18)) {   
    Serial.println("Couldnt start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  // range in Gs (2 means from -2G to 2G)
  lis.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G

  Serial.print("Range = "); Serial.print(2 << lis.getRange());
  Serial.println("G");

  // 400Hz datarate
  lis.setDataRate(LIS3DH_DATARATE_400_HZ);
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

  // set up the LCD's number of rows and columns
  if (!lcd.begin(16, 2)) {
    Serial.println("Could not init backpack. Check wiring.");
    while(1);
  }
  Serial.println("Backpack init'd.");

  // create custom character
  lcd.createChar(0, degree);
  // clear lcd
  lcd.clear();

  // Print "Acceleration" in first row
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

// moving average of 15
  int i;
  for(i = 0; i <= 13; i++){
    xarr[i] = xarr[i + 1];
    yarr[i] = yarr[i + 1];
    zarr[i] = zarr[i + 1];
  }


  xarr[14] = lis.x;
  yarr[14] = lis.y;
  zarr[14] = lis.z;


  long sumx = 0;
  long sumy = 0;
  long sumz = 0;
  for(i = 0; i <= 14; i++){
    sumx += xarr[i];
    sumy += yarr[i];
    sumz += zarr[i];
  }
  Serial.println();

  long x = sumx/15;
  long y = sumy/15;
  long z = sumz/15;


  // Apply offsets
  x += 80;
  y += 600;
  
  // // Print averaged values
  // Serial.print("X:  "); Serial.print(x);
  // Serial.print("  \tY:  "); Serial.print(y);
  // Serial.print("  \tZ:  "); Serial.print(z);
  // Serial.println();

  // Calculate angles (use int instead of double for now)
  int rho = atan(x / sqrt(pow(y,2) + pow(z,2) )) * (180/PI);
  int phi = atan(y / sqrt(pow(x,2) + pow(z,2) )) * (180/PI);
  int theta = atan(sqrt(pow(x,2) + pow(y,2) ) / z) * (180/PI);

    // Print angles
  Serial.print("Rho:  "); Serial.print(rho);
  Serial.print("  \tPhi:  "); Serial.print(phi);
  Serial.print("  \tTheta:  "); Serial.print(theta);
  Serial.println();






  // lcd.setCursor(14, 0);
  // lcd.print(count);
  count++;

  // If you print too fast, the screen is unreadable
  if (count == 15){
    lcd.setCursor(0, 1);
    lcd.print("                ");

    // // Print averaged values on LCD
    // lcd.setCursor(0, 1);
    // lcd.print(x/10);  // divide by 10 to truncate the least significant digit
    // lcd.setCursor(5, 1);
    // lcd.print(y/10);
    // lcd.setCursor(10, 1);
    // lcd.print(z/10);

    // Print angles on LCD
    lcd.setCursor(0, 1);
    lcd.print(rho); lcd.print((char)0); // divide by 10 to truncate the least significant digit
    lcd.setCursor(5, 1);
    lcd.print(phi); lcd.print((char)0);
    lcd.setCursor(10, 1);
    lcd.print(theta); lcd.print((char)0);
    
    count = 0;
  }


  delay(10);
}
