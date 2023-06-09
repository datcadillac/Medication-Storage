/****************************************************************************************  
**  This is example LINX firmware for use with the Arduino Uno with the serial 
**  interface enabled.
**
**  For more information see:           www.labviewmakerhub.com/linx
**  For support visit the forums at:    www.labviewmakerhub.com/forums/linx
**  
**  Written By Sam Kristoff
**
**  BSD2 License.
****************************************************************************************/

//Include All Peripheral Libraries Used By LINX
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <Servo.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

//Include Device Specific Header From Sketch>>Import Library (In This Case LinxChipkitMax32.h)
//Also Include Desired LINX Listener From Sketch>>Import Library (In This Case LinxSerialListener.h)
#include <LinxArduinoUno.h>
#include <LinxSerialListener.h>
 
int dht(); 

//Create A Pointer To The LINX Device Object We Instantiate In Setup()
LinxArduinoUno* LinxDevice;
// Wiring: SDA pin is connected to A4 and SCL pin to A5.
// Connect to LCD via I2C, default address 0x27 (A0-A2 not jumpered)
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2); // Change to (0x27,20,4) for 20x4 LCD.


//Initialize LINX Device And Listener
void setup()
{
  //Instantiate The LINX Device
  LinxDevice = new LinxArduinoUno();

  // Initiate the LCD:
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  //The LINXT Listener Is Pre Instantiated, Call Start And Pass A Pointer To The LINX Device And The UART Channel To Listen On
  LinxSerialConnection.Start(LinxDevice, 0);  
  LinxSerialConnection.AttachCustomCommand(2, dht);
  LinxSerialConnection.AttachCustomCommand(3, drawTEMP);
  LinxSerialConnection.AttachCustomCommand(4, drawHUM);
}

void loop()
{
  //Listen For New Packets From LabVIEW
  LinxSerialConnection.CheckForCommands();
  
  //Your Code Here, But It will Slow Down The Connection With LabVIEW
  
}

int dht(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
  if (numInputBytes != 1)
  {
    return 1;
  }

  DHT dht(input[0], DHT11);

  dht.begin();
  delay(2000);

  unsigned char h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  unsigned char t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  unsigned char f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    return 1;
  }

  unsigned char hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  unsigned char hic = dht.computeHeatIndex(t, h, false);

  response[0] = h;
  response[1] = t;
  response[2] = f;
  response[3] = hic;
  response[4] = hif;

  *numResponseBytes = 5;

  return 0;
}

int drawTEMP(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
  lcd.setCursor(0, 0); // Set the cursor on the third column and first row.
  lcd.print("Temperature: ");
  lcd.print(input[0]);
   
  response[0] = 1;

  *numResponseBytes = numInputBytes;
return 0;
}

int drawHUM(unsigned char numInputBytes, unsigned char* input, unsigned char* numResponseBytes, unsigned char* response)
{
  lcd.setCursor(0, 1); // Set the cursor on the third column and first row.
  lcd.print("Humidity: ");
  lcd.print(input[0]);
   
  response[0] = 1;

  *numResponseBytes = numInputBytes;
return 0;
}
