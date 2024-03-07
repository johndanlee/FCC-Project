#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Math.h>

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();



// include the library code:
#include "Adafruit_LiquidCrystal.h"

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);


// for moving average
int count = 0;
int xarr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int yarr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int zarr[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


// byte map for degree symbol
uint8_t degree[8] = {0xc,0x12,0x12,0xc,0x0,0x0,0x0};



// constants won't change. They're used here to set pin numbers:
const int buttonPin = 2;  // the number of the pushbutton pin
const int ledPin = 13;    // the number of the LED pin

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status




// Offsets
int x_offset = 0;
int y_offset = 0;
int z_offset = 0;



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







  // set up the LCD's number of rows and columns:
  if (!lcd.begin(16, 2)) {
    Serial.println("Could not init backpack. Check wiring.");
    while(1);
  }
  Serial.println("Backpack init'd.");

  // create custom character
  lcd.createChar(0, degree);
  // clear lcd
  lcd.clear();
  // write custom character

  lcd.setCursor(0, 0);
  lcd.print("Angle in Degrees");



  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);


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








  // BUTTON CODE
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    delay(1000);
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    x_offset = 0 - x;
    y_offset = 0 - y;
    z_offset = 16384 - z;

    lcd.setCursor(0, 0);
    lcd.print("Calibrated!     ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(1000);
    lcd.setCursor(0, 0);
    lcd.print("Angle in degrees");

    // Print offsets
    Serial.print("Xo:  "); Serial.print(x_offset);
    Serial.print("  \tYo:  "); Serial.print(y_offset);
    Serial.print("  \tZo:  "); Serial.print(z_offset);
    Serial.println();
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
  }








  // Apply offsets
  x += x_offset;
  y += y_offset;
  z += z_offset;
  
  // Print averaged values
  Serial.print("X:  "); Serial.print(x);
  Serial.print("  \tY:  "); Serial.print(y);
  Serial.print("  \tZ:  "); Serial.print(z);
  Serial.println();

  // Calculate angles (use int instead of double for now)
  int rho = atan(x / sqrt(pow(y,2) + pow(z,2) )) * (180/PI);
  int phi = atan(y / sqrt(pow(x,2) + pow(z,2) )) * (180/PI);
  int theta = atan(sqrt(pow(x,2) + pow(y,2) ) / z) * (180/PI);

  // // Print angles
  // Serial.print("Rho:  "); Serial.print(rho);
  // Serial.print("  \tPhi:  "); Serial.print(phi);
  // Serial.print("  \tTheta:  "); Serial.print(theta);
  // Serial.println();

  // Print offsets
  Serial.print("Xo:  "); Serial.print(x_offset);
  Serial.print("  \tYo:  "); Serial.print(y_offset);
  Serial.print("  \tZo:  "); Serial.print(z_offset);
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









  delay(15);
}
