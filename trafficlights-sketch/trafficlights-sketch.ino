/**
 * Array with the three pins for red, green and blue.
 */
const byte pins[] = { 3, 5, 6 };

byte color_red[] = { 255, 0, 0 };
byte color_green[] = { 0, 255, 0 };
byte color_yellow[] = { 255, 75, 0 };
byte color_blue[] = { 0, 0, 255 };
byte color_off[] = { 0, 0, 0 };
byte color_white[] = { 255, 120, 120 };

byte currentColor[3];

/**
 * Initializes the pins, turns all LED outputs off and starts a serial
 * connection with 57600 bauds.
 */
void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], 0);
  }
  Serial.begin(57600);
}

/**
 * If there is input available on the serial line, it is read byte by 
 * byte. Each byte is evaluated and a color change occurs if the byte
 * designates one of the preprogrammed colors.
 */
void loop() {
  while(Serial.available() > 0) {
    byte b = Serial.read();
    switch(b) {
      case 'o':
        changeColorAndBlink(color_off);
        break;
      case 'r':
        changeColorAndBlink(color_red);
        break;
      case 'y':
        changeColorAndBlink(color_yellow);
        break;
      case 'g':
        changeColorAndBlink(color_green);
        break;
      case 'b':
        changeColorAndBlink(color_blue);
        break;
      case 'w':
        changeColorAndBlink(color_white);
        break;
      case 't':
      default:
        byte previousColor[3] = { currentColor[0], currentColor[1], currentColor[2] };
        crossFade(color_off);
        crossFade(color_red);
        crossFade(color_green);
        crossFade(color_blue);
        crossFade(color_white);
        crossFade(previousColor);
        break;
    }
  }
}

/**
 * Changes the current color to the new given color. The transition is done by
 * blinking the current color two times, then blinking the new color two times
 * before going solid with the new color.
 * The color will only change when the new color is different from the previous
 * one. If the is no change in color, this method will do nothing.
 * 
 * @param color
 *     the color to which to switch to
 */
void changeColorAndBlink(byte color[3]) {
  // Only update when there is a real change
  if (!(currentColor[0] == color [0] && currentColor[1] == color[1] && currentColor[2] == color[2])) {
  
    for (int i = 0; i < 2; i++) {
      setColor(color_off);
      delay(600);
      setColor(currentColor);
      delay(600);
    }
    for (int i = 0; i < 2; i++) {
      setColor(color_off);
      delay(600);
      setColor(color);
      delay(600);
    }
    currentColor[0] = color[0];
    currentColor[1] = color[1];
    currentColor[2] = color[2];
  }
}

/**
 * Applies the given color to the RGB LEDs using the defined PWM pins.
 * 
 * @param color
 *     array with the three values for red, green and blue. They range
 *     from 255 (full brightness) to 0 (off)
 */
void setColor(byte *color) {
  for (int i = 0; i < 3; i++) {
    analogWrite(pins[i], color[i]);
  }
}

/* 
 * ===================================================================
 * The following code was taken from the ColorCrossfader Tutorial at
 * https://www.arduino.cc/en/Tutorial/ColorCrossfader and modified to
 * work with my color handling.
 * ===================================================================
 */

int wait = 2;      // 2ms internal crossFade delay; increase for slower fades
int hold = 0;      // Optional hold when a color is complete, before the next crossFade

// Initialize color variables
int prevR = 0;
int prevG = 0;
int prevB = 0;

/**
 * Calculates the step size between the {@code prevValue} and the given
 * {@code endValue} with regard to a fixed iteration count of 1020 so that
 * each transition will take the same time, no matter between what colors
 * the fade occurs.
 * 
 * @param prevValue
 *     the previous color value
 * @param endValue
 *     the next color value
 * @param
 *     1020 divided by the step size (may be negative)
 */
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

/** 
 * The next function is calculateVal. When the loop value, i,
 * reaches the step size appropriate for one of the
 * colors, it increases or decreases the value of that color by 1. 
 * (R, G, and B are each calculated separately.)
 * 
 * @param step
 *     the size of the desired step
 * @param val
 *     the current color value (as a basis for calculation)
 * @param i
 *     the current iteration
 * @return
 *     the calculated new value (may be the same as {@code val} if
 *     the threshold for a new value has not yet been reached
 */
int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/** 
 * crossFade() converts the percentage colors to a 
 * 0-255 range, then loops 1020 times, checking to see if  
 * the value needs to be updated each time, then writing
 * the color values to the correct pins.
 * 
 * @param color
 *     the color to fade to
 */

void crossFade(byte color[3]) {
  // Convert to 0-255
  int R = color[0];
  int G = color[1];
  int B = color[2];

  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);

  for (int i = 0; i <= 1020; i++) {
    currentColor[0] = calculateVal(stepR, currentColor[0], i);
    currentColor[1] = calculateVal(stepG, currentColor[1], i);
    currentColor[2] = calculateVal(stepB, currentColor[2], i);

    analogWrite(pins[0], currentColor[0]);   // Write current values to LED pins
    analogWrite(pins[1], currentColor[1]);      
    analogWrite(pins[2], currentColor[2]); 

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop
  }
  // Update current values for next loop
  prevR = currentColor[0]; 
  prevG = currentColor[1]; 
  prevB = currentColor[2];
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}

