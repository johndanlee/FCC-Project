// constants won't change. They're used here to set pin numbers:
const int configButtonPin = 2;  // the number of the configuration pushbutton pin
const int selectButtonPin = 3;  // the number of the selector pushbutton pin
const int ledPin = 13;    // the number of the LED pin

// variables will change:
int configButtonState = 0;  // variable for reading the pushbutton status
int selectButtonState = 0;

void setup(void) {
  // 115200 baud rate
  Serial.begin(115200);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the button pins as inputs:
  pinMode(configButtonPin, INPUT);
  pinMode(selectButtonPin, INPUT);

}



void loop() {

  if (digitalRead(selectButtonPin) == HIGH) {
    digitalWrite(ledPin, HIGH);

    unsigned int i = 0;
    // keep track of which table is selected, maybe change to use enum?
    // m430 = 0, m385 = 1, m918 = 2, xm918 = 3
    unsigned int t = 0;
    String arr[4] = {"a", "bc", "de", "fg"};
    while(i < 1000) {
      Serial.print(digitalRead(selectButtonPin)); Serial.print("   "); Serial.println(arr[t]);

      if (digitalRead(selectButtonPin) == HIGH && t <= 3) {
        t++;
      } else {
        t = 0;
      }

      i++;
    }
    digitalWrite(ledPin, LOW);
  }
}


