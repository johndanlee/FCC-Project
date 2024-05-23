
#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>
#include <Math.h>
#include "flight_tables.h"

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


// byte map for degree symbol
uint8_t degree[8] = {0xc,0x12,0x12,0xc,0x0,0x0,0x0};



// constants won't change. They're used here to set pin numbers:
const int configButtonPin = 2;  // the number of the configuration pushbutton pin
const int selectButtonPin = 3;  // the number of the selector pushbutton pin
const int ledPin = 13;    // the number of the LED pin

// variables will change:
int configButtonState = 0;  // variable for reading the pushbutton status
int selectButtonState = 0;
unsigned int tableState = 0;         // use to track table state
String tableStrings[4] = {"m430            ", "m385            ", "m918            ", "xm918           "};


// Offsets
int x_offset = 0;
int y_offset = 0;
int z_offset = 0;



void setup(void) {
  // 115200 baud rate
  Serial.begin(115200);

  // pause until serial console opens
  // while (!Serial) delay(10);

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

  // Print first row
  lcd.setCursor(0, 0);
  lcd.print("Mils       Range");



  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the button pins as inputs:
  pinMode(configButtonPin, INPUT);
  pinMode(selectButtonPin, INPUT);

}



void loop() {

  if (digitalRead(selectButtonPin) == HIGH) {
    digitalWrite(ledPin, HIGH);
    lcd.setCursor(0, 0);
    lcd.print(tableStrings[tableState] + "          ");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    delay(100);

    unsigned int i = 0;
    while(i < 130) {
      // Serial.print(digitalRead(selectButtonPin)); Serial.print("   "); Serial.print(tableState); Serial.print("   "); Serial.println(tableStrings[tableState]);

      if (digitalRead(selectButtonPin) == HIGH) {
        if (tableState >= 3) {
          tableState = 0;
        } else {
          tableState++;
        }

        lcd.setCursor(0, 0);
        lcd.print(tableStrings[tableState] + "          ");
        lcd.setCursor(0, 1);
        lcd.print("                ");

        i = 0;
        delay(200);
      }

      i++;
      delay(15);
    }

    digitalWrite(ledPin, LOW);
    lcd.setCursor(0, 0);
    lcd.print("Mils       Range");
  }


  lis.read();      // get X Y and Z data

  // // Check arrays
  // Serial.print("Before copy: "); 
  // for(int i = 0; i < 5; i++){Serial.print(xarr[i]); Serial.print(" ");}
  // Serial.print("\t");
  // for(int i = 0; i < 5; i++){Serial.print(yarr[i]); Serial.print(" ");}
  // Serial.print("\t");
  // for(int i = 0; i < 5; i++){Serial.print(zarr[i]); Serial.print(" ");}
  // Serial.println();


// shift first 14 elements of the array one index to the left
  int i;
  for(i = 0; i <= 13; i++){
    xarr[i] = xarr[i + 1];
    yarr[i] = yarr[i + 1];
    zarr[i] = zarr[i + 1];
  }

  // update last element with the latest acceleration value
  xarr[14] = lis.x;
  yarr[14] = lis.y;
  zarr[14] = lis.z;

  // sum all elements
  long sumx = 0;
  long sumy = 0;
  long sumz = 0;
  for(i = 0; i <= 14; i++){
    sumx += xarr[i];
    sumy += yarr[i];
    sumz += zarr[i];
  }

  // average = sum/15
  long x = sumx/15;
  long y = sumy/15;
  long z = sumz/15;








  // BUTTON CODE
  // read the state of the pushbutton value:
  configButtonState = digitalRead(configButtonPin);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (configButtonState == HIGH) {
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
    lcd.print("Mils       Range");

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
  // Serial.print("X:  "); Serial.print(x);
  // Serial.print("  \tY:  "); Serial.print(y);
  // Serial.print("  \tZ:  "); Serial.print(z);
  // Serial.println();

  // Calculate angles (use int instead of float for now)
  // divide by 1000 to get milliradians
  float rho = atan(x / sqrt(pow(y,2) + pow(z,2) ))   *1000;
  float phi = atan(y / sqrt(pow(x,2) + pow(z,2) ))   *1000;
  float theta = atan(sqrt(pow(x,2) + pow(y,2) ) / z) *1000;

  // // Print angles
  Serial.print("Rho:  "); Serial.print(rho);
  Serial.print("  \tPhi:  "); Serial.print(phi);
  Serial.print("  \tTheta:  "); Serial.print(theta);
  Serial.println();

  // Print offsets
  // Serial.print("Xo:  "); Serial.print(x_offset);
  // Serial.print("  \tYo:  "); Serial.print(y_offset);
  // Serial.print("  \tZo:  "); Serial.print(z_offset);
  // Serial.println();






  // lcd.setCursor(14, 0);
  // lcd.print(count);
  count++;

  // If you print too fast, the screen is unreadable
  if (count == 15){
    count = 0;

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
    lcd.print(round(rho));
    // lcd.setCursor(5, 1);
    // lcd.print(round(phi));
    // lcd.setCursor(10, 1);
    // lcd.print(round(theta));
    
    // Print range on LCD
    lcd.setCursor(10, 1);
    lcd.print((int) find_range(rho, tableState)); lcd.print("m"); // cast to int to truncate decimal
  }


  delay(15);
}
