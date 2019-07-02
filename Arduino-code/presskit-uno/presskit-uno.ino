#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// OLED DISPLAY BAR CHART SIZE
int barWidth = 7;
int barHeight = 45;
int barSize = barWidth * barHeight;

#include <CapacitiveSensor.h>
CapacitiveSensor   cs_6_2 = CapacitiveSensor(6, 2);
CapacitiveSensor   cs_6_3 = CapacitiveSensor(6, 3);
CapacitiveSensor   cs_6_4 = CapacitiveSensor(6, 4);
CapacitiveSensor   cs_6_5 = CapacitiveSensor(6, 5);
const int capBuffer = 2; // CHANGE THIS VALUE TO SET DEGREE OF "SMOOTHING" FOR CAP SENSE INPUTS. 1 = NO SMOOTHING
unsigned long capRunningAvg2[capBuffer];
unsigned long capRunningAvg3[capBuffer];
unsigned long capRunningAvg4[capBuffer];
unsigned long capRunningAvg5[capBuffer];
unsigned long capInitMax = 1000;
unsigned long capMax2 = capInitMax;
unsigned long capMax3 = capInitMax;
unsigned long capMax4 = capInitMax;
unsigned long capMax5 = capInitMax;

const int anaBuffer = 2; // CHANGE THIS VALUE TO SET DEGREE OF "SMOOTHING" FOR ANALOG INPUTS. 1 = NO SMOOTHING
unsigned long anaRunningAvg0[anaBuffer];
unsigned long anaRunningAvg1[anaBuffer];
unsigned long anaRunningAvg2[anaBuffer];
unsigned long anaRunningAvg3[anaBuffer];

void setup() {

  // SET CAP SENSE TIMEOUT, LOW VALUE TO MINIMIZE READING TIME
  cs_6_2.set_CS_Timeout_Millis(50);
  cs_6_3.set_CS_Timeout_Millis(50);
  cs_6_4.set_CS_Timeout_Millis(50);
  cs_6_5.set_CS_Timeout_Millis(50);

  // INIT INPUT VALUE ARRAYS
  for (int i = 0; i < capBuffer; i++) {
    capRunningAvg2[i] = 0;
    capRunningAvg3[i] = 0;
    capRunningAvg4[i] = 0;
    capRunningAvg5[i] = 0;
  }
  for (int i = 0; i < anaBuffer; i++) {
    anaRunningAvg0[i] = 0;
    anaRunningAvg1[i] = 0;
    anaRunningAvg2[i] = 0;
    anaRunningAvg3[i] = 0;
  }

  // INIT OLED DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();

  // INIT SERIAL COMMUNICATION AT 9600 BAUD RATE
  Serial.begin(9600);
}

void loop() {

  // SHIFT INPUT VALUE ARRAYS UP BY ONE
  shiftArray(capRunningAvg2, capBuffer);
  shiftArray(capRunningAvg3, capBuffer);
  shiftArray(capRunningAvg4, capBuffer);
  shiftArray(capRunningAvg5, capBuffer);
  shiftArray(anaRunningAvg0, anaBuffer);
  shiftArray(anaRunningAvg1, anaBuffer);
  shiftArray(anaRunningAvg2, anaBuffer);
  shiftArray(anaRunningAvg3, anaBuffer);

  // UPDATE FIRST VALUE IN EACH INPUT ARRAY WITH SENSOR READING
  capRunningAvg2[0] = cs_6_2.capacitiveSensor(30);
  capRunningAvg3[0] = cs_6_3.capacitiveSensor(30);
  capRunningAvg4[0] = cs_6_4.capacitiveSensor(30);
  capRunningAvg5[0] = cs_6_5.capacitiveSensor(30);
  anaRunningAvg0[0] = analogRead(A0);
  anaRunningAvg1[0] = analogRead(A1);
  anaRunningAvg2[0] = analogRead(A2);
  anaRunningAvg3[0] = analogRead(A3);

  // AVERAGE ALL VALUES IN EACH ARRAY. AKA SMOOTHING FUNCTION
  unsigned long capAvg2 = averageArray(capRunningAvg2, capBuffer);
  unsigned long capAvg3 = averageArray(capRunningAvg3, capBuffer);
  unsigned long capAvg4 = averageArray(capRunningAvg4, capBuffer);
  unsigned long capAvg5 = averageArray(capRunningAvg5, capBuffer);
  unsigned long anaAvg0 = averageArray(anaRunningAvg0, anaBuffer);
  unsigned long anaAvg1 = averageArray(anaRunningAvg1, anaBuffer);
  unsigned long anaAvg2 = averageArray(anaRunningAvg2, anaBuffer);
  unsigned long anaAvg3 = averageArray(anaRunningAvg3, anaBuffer);

  // UPDATE MAX VALUE FOR CAP SENSE INPUTS. FOR VISUALIZATION
  capMax2 = capAvg2 > capInitMax ? (capMax2 * 2 + capAvg2) / 3 : (capMax2 * 2 + capInitMax) / 3;
  capMax3 = capAvg3 > capInitMax ? (capMax3 * 2 + capAvg3) / 3 : (capMax3 * 2 + capInitMax) / 3;
  capMax4 = capAvg4 > capInitMax ? (capMax4 * 2 + capAvg4) / 3 : (capMax4 * 2 + capInitMax) / 3;
  capMax5 = capAvg5 > capInitMax ? (capMax5 * 2 + capAvg5) / 3 : (capMax5 * 2 + capInitMax) / 3;


  // VISUALIZATION
  int disp1 = map(capAvg2, 100, capMax2, barWidth, barSize);
  disp1 = constrain(disp1, barWidth, barSize);
  int disp2 = map(capAvg3, 100, capMax3, barWidth, barSize);
  disp2 = constrain(disp2, barWidth, barSize);
  int disp3 = map(capAvg4, 100, capMax4, barWidth, barSize);
  disp3 = constrain(disp3, barWidth, barSize);
  int disp4 = map(capAvg5, 100, capMax5, barWidth, barSize);
  disp4 = constrain(disp4, barWidth, barSize);
  int disp5 = map(anaAvg3, 0, 1023, barWidth, barSize);
  disp5 = constrain(disp5, barWidth, barSize);
  int disp6 = map(anaAvg2, 0, 1023, barWidth, barSize);
  disp6 = constrain(disp6, barWidth, barSize);
  int disp7 = map(anaAvg1, 0, 1023, barWidth, barSize);
  disp7 = constrain(disp7, barWidth, barSize);
  int disp8 = map(anaAvg0, 0, 1023, barWidth, barSize);
  disp8 = constrain(disp8, barWidth, barSize);

  display.clearDisplay();

  displayGraph(0, 0, "02 ", disp1, capAvg2, barWidth, barSize);
  displayGraph(0, 8, "03 ", disp2, capAvg3, barWidth, barSize);
  displayGraph(0, 16, "04 ", disp3, capAvg4, barWidth, barSize);
  displayGraph(0, 24, "05 ", disp4, capAvg5, barWidth, barSize);

  displayGraph(70, 0, "A3 ", disp5, anaAvg3, barWidth, barSize);
  displayGraph(70, 8, "A2 ", disp6, anaAvg2, barWidth, barSize);
  displayGraph(70, 16, "A1 ", disp7, anaAvg1, barWidth, barSize);
  displayGraph(70, 24, "A0 ", disp8, anaAvg0, barWidth, barSize);

  display.display();

  // SERIAL COMMUNICATION
  Serial.print(capAvg2);
  Serial.print(" ");
  Serial.print(capAvg3);
  Serial.print(" ");
  Serial.print(capAvg4);
  Serial.print(" ");
  Serial.print(capAvg5);
  Serial.print(" ");
  Serial.print(anaAvg3);
  Serial.print(" ");
  Serial.print(anaAvg2);
  Serial.print(" ");
  Serial.print(anaAvg1);
  Serial.print(" ");
  Serial.print(anaAvg0);
  Serial.println();

  delay(20);
}

void shiftArray(unsigned long *arr, int len) {
  for (int i = len - 1; i > 0; i--) {
    arr[i] = arr[i - 1];
  }
}

unsigned long averageArray(unsigned long *arr, int len) {
  unsigned long arrSum = 0;
  for (int i = 0; i < len; i++) {
    arrSum = arrSum + arr[i];
  }
  return arrSum / len;
}

void displayGraph(int pX, int pY, String s, long valDisp, long val, int bW, int bS) {
  display.setTextColor(WHITE);
  display.setCursor(pX, pY);
  display.print(s);
  for (int i = 0; i < valDisp; i++) {
    display.drawPixel(pX + 14 + i / bW, pY + i % bW, WHITE);
  }

  String sVal = (String)val;
  int sLen = sVal.length() + 1;
  char charArr[sLen];
  sVal.toCharArray(charArr, sLen);
  int dLen = (valDisp/(bW*6));
  for (int i=0; i<sLen; i++) {
    if (i < dLen) {
      display.setTextColor(BLACK);
    } else {
      display.setTextColor(WHITE);
    }
    display.print(charArr[i]);
  }
  
//  if (valDisp > bS * 0.1) {
//    display.setTextColor(BLACK);
//  } else {
//    display.setTextColor(WHITE);
//  }
//  display.print(val);
}
