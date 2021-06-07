/**
 * ============================================================================
 * Controllable Traffic Lights v1.1.0
 * ============================================================================
 * 
 * Creative Commons License
 * ------------------------
 * Controllable Traffic Lights by Hendrik Busch is licensed under a 
 * Creative Commons Attribution-ShareAlike 4.0 International License.
 * 
 * This sketch drives one or more RGB LEDs where each color is connected to a
 * different pin (see {@link #rgbPins}). The sketch takes in serial data a 
 * 57600 bauds and listens to single character commands that correspond to
 * the preprogrammed colors. Currently the following colors are supported:
 * 
 * - o = off/black
 * - g = green
 * - y = yellow
 * - r = red
 * - b = blue
 * - t = test
 * 
 * Further colors can easily be added by extending the main switch block.
 * 
 * I use this circuit as health monitor for one of my applications, so I
 * also added the notion of health levels to the code, meaning yellow is
 * worse than green and red is worse than yellow (and vice versa). Each 
 * change to one of these three colors is accompanied by a small jingle
 * that is played on an attached speaker. The jingle signals to the user
 * if the health is improving or deteriorating even without the user
 * looking at the displayed color.
 */

/**
 * Array with the three pins for red, green and blue.
 */
const byte rgbPins[] = { 3, 5, 6 };

/**
 * The pin to which the speaker is connected to. Make sure
 * that you prevent low impedance speakers from drawing to
 * much current before connecting.
 */
const byte speakerPin = 8;

byte color_red[] = { 255, 0, 0 };
byte color_green[] = { 0, 255, 0 };
byte color_yellow[] = { 255, 75, 0 };
byte color_blue[] = { 0, 0, 255 };
byte color_off[] = { 0, 0, 0 };
byte color_white[] = { 255, 120, 120 };

byte currentColor[3];
byte currentByte = 'o';

/**
 * The jingle to play when the displayed states deteriorates (i.e. from
 * green to yellow or red or from yellow to red).
 */
char notesStateDown[] = "a a g ";

/**
 * The jingle to play when the displayed state improves (i.e. from either
 * yellow or red to green).
 */
char notesStateUp[] = "g g a ";

/**
 * The jingle to play when the state changes to either red, green or yellow
 * from a previous state that was none of these colors.
 */
char notesStateUnknown[] = "  g   ";

/**
 * Initializes the pins, turns all LED outputs off and starts a serial
 * connection with 57600 bauds.
 */
void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(rgbPins[i], OUTPUT);
    digitalWrite(rgbPins[i], 0);
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
    if (currentByte != b)
    {
      switch(b) {
        case 'o':
          changeColorAndBlink(color_off);
          break;
        case 'r':
          if (currentByte == 'y' || currentByte == 'g') {
            playJingle(notesStateDown);
          }
          else {
            playJingle(notesStateUnknown);
          }
          changeColorAndBlink(color_red);          
          break;
        case 'y':
          if (currentByte == 'r') {
            playJingle(notesStateUp);
          }
          else if (currentByte == 'g'){
            playJingle(notesStateDown);
          }
          else {
            playJingle(notesStateUnknown);
          }
          changeColorAndBlink(color_yellow);
          break;
        case 'g':
          if (currentByte == 'y' || currentByte == 'r') {
            playJingle(notesStateUp);
          }
          else {
            playJingle(notesStateUnknown);
          }          
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
      currentByte = b;
    }
  }
}

/**
 * Changes the current color to the new given color. The transition is done by
 * blinking the current color two times, then blinking the new color two times
 * before going solid with the new color.
 * This method does not check whether the given color is already displayed. The
 * caller of this method is responsible for preventing no-change operations.
 * 
 * @param color
 *     the color to which to switch to
 */
void changeColorAndBlink(byte color[3]) {
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

/**
 * Applies the given color to the RGB LEDs using the defined PWM pins.
 * 
 * @param color
 *     array with the three values for red, green and blue. They range
 *     from 255 (full brightness) to 0 (off)
 */
void setColor(byte *color) {
  for (int i = 0; i < 3; i++) {
    analogWrite(rgbPins[i], color[i]);
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

    analogWrite(rgbPins[0], currentColor[0]);   // Write current values to LED pins
    analogWrite(rgbPins[1], currentColor[1]);      
    analogWrite(rgbPins[2], currentColor[2]); 

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop
  }
  // Update current values for next loop
  prevR = currentColor[0]; 
  prevG = currentColor[1]; 
  prevB = currentColor[2];
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}

/* 
 * ===================================================================
 * The following code was taken from the Melody Tutorial at
 * https://www.arduino.cc/en/Tutorial/Melody and modified for this
 * sketch to work with small structured jingles.
 * ===================================================================
 */

/**
 * The beat pattern for all jingles. Equals short, short, long.
 */
float beats[] = { 1, 1, 1, 1, 2, 2 };

/**
 * The tempo a which to play. An increase in tempo actually decreases
 * the playback speed.
 */
int tempo = 100;

/**
 * Plays the given jingle that consists of 6 notes/rests.
 * 
 * @param jingle
 *     char array with 6 notes/rests to play
 */
void playJingle(char jingle[6]) {
  for (int i = 0; i < 6; i++) {
    if (jingle[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(jingle[i], beats[i] * tempo);
    }
    
    // pause between notes
    delay(tempo / 2); 
  }
}

/**
 * Plays the given tone for the given duration. The duration is used to calculate
 * the timing to create a sine wave with the frequency of the given tone.
 * 
 * @param ton1 
 *     the tone to play (in Hz)
 * @param duration
 *     the duration for which to play the tone (equals beat count times tempo)
 */
void playTone(int ton1, int duration) {
  for (long i = 0; i < duration * 1000L; i += ton1) {
    tone(speakerPin, ton1);
    delayMicroseconds(ton1);
  }
  noTone(speakerPin);
}

/**
 * Plays the given note for the given duration. The note is converted into a frequency
 * using the internal lookup table.
 * 
 * @param note
 *     the note to play (refer to lookup table for valid notes)
 * @param duration
 *     the duration for which to play the note (equals beat count times tempo)
 */
void playNote(char note, int duration) {
//                        c    c#   d    d#   e    f    f#   g    g#   a    a#   b
  char names[] = { ' ',  '!', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'i', 'N', 'R', 'u',  '1', 'L', 'k'}; // [i = b flat] [N = G flat] [R = D#] [u = g#] [1 = C oct. 5] [L = E flat]
  int tones[] =  {   0, 1046, 138, 146, 155, 164, 174, 184, 195, 207, 220, 233, 246, 261, 293, 329, 349, 391, 440, 493, 523, 587, 659, 698, 783, 880, 987, 466, 740, 622, 415, 1046, 622u, 227};
  
  // play the tone corresponding to the note name
  for (int i = 0; i < 34; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}
