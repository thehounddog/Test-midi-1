#include <MIDI.h>
#include <ezButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LED 2

int rc600Channel = 2;
int tempoCC = 7;
int startCC = 3;
int tempoVel = 0;
int reqBPM = 0;

ezButton start_button(15);  // create ezButton pin 15;
ezButton tempo_up_button(19);  // pin 19;
ezButton tempo_down_button(18);  // pin 18;
LiquidCrystal_I2C lcd(0x27, 16, 2);

MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI); // originally MIDI_CREATE_DEFAULT_INSTANCE();

// ============ TEMPO CALC VARIABLES =============
byte midi_clock = 0xf8;
unsigned long int startTime;
int beatCount;
bool counting = false;
long int bpm;
long int noteTime;
// ========================

void updatescreen(){ // Used to update screen when a button is pressed.
    lcd.clear();  // clears the display to print new message
    lcd.setCursor(0, 0); // set cursor to first column, second row
    lcd.print("TX " +String(reqBPM) +  " CC" + String(tempoCC) + " V" + String(tempoVel));
}
// ==========TEMPO CALC ================
void tempoCalculator(){
    byte type = MIDI.getType();
    if (type == midi_clock) {
      if ( not counting) { // first time you have seen a clock message in this batch
        startTime = millis();
        counting = true;
        beatCount = 0; 
      }
      else {
        beatCount += 1;  // short for ( beatcount = beatcount + 1 )
        if (beatCount >= 96) {
          noteTime = millis() - startTime;
          counting = false;
          // you now have the time for one note so calculate the BPM
          bpm = 240110/noteTime; // was 24000 changed to correct time slippage
          lcd.setCursor(0, 1); // set cursor to first column, second row
          lcd.print("LOOPER BPM " + String(bpm) + " ");
        }
      }
    }
}
// ======== TEMPO END ====================== 

// ========= CHANGE TEMPO FUNCTION ==============
void changeTempo(int bpmChange){
    reqBPM = bpm + bpmChange; // Set target BPM
    Serial.println("BPM " + String(bpm) + " reqBPM " + String(reqBPM));
    digitalWrite(LED, HIGH);

    if(reqBPM >= 40 && reqBPM <= 160){
      tempoVel = reqBPM - 40;
      tempoCC = 4;
      MIDI.sendControlChange(tempoCC, tempoVel, rc600Channel);
      updatescreen();
    }
    if(reqBPM >= 160 && reqBPM <= 287){
      tempoVel = reqBPM - 160;
      tempoCC = 5;
      MIDI.sendControlChange(tempoCC, tempoVel, rc600Channel);
      updatescreen();
    }
    if(reqBPM < 40 || reqBPM > 287){
      lcd.setCursor(0, 0);
      lcd.print("BPM out of range");
      Serial.println("BPM outside of range");
    }
}    
// ========== END CHANGE TEMPO ===========

void setup()
{
  pinMode(LED, OUTPUT);
  start_button.setDebounceTime(50); 
  tempo_up_button.setDebounceTime(50); // set debounce time to 50 milliseconds
  tempo_down_button.setDebounceTime(50);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Other examples had - MIDI_CHANNEL_OFF works too. Not sure which one to use
  
  Serial.begin(9600); // No clash as MIDI on Serial1

  // LCD DISPLAY
  lcd.init(); // initialize LCD
  lcd.backlight(); // turn on LCD backlight
  lcd.setCursor(0, 0); // set cursor to first column, first row
  lcd.print("RAT600 MIDI");        
}

void loop(){

  start_button.loop(); // MUST call to get button states
  tempo_up_button.loop(); 
  tempo_down_button.loop();

// ============ TEMPO CALC =============
  if (MIDI.read()) {   // Read incoming MIDI
    tempoCalculator();
  }
// ============ TEMPO CALC END SECTION=============



// ====== Check if the START button is pressed ========
  if(start_button.isPressed()) {
    Serial.println("The button 1 is pressed");

    // Send a Note On message when the button is pressed
    digitalWrite(LED, HIGH);
    //MIDI.sendNoteOn(60, 127, 1); // Middle C
    MIDI.sendControlChange(startCC, 127, rc600Channel);
    lcd.clear();  // clears the display to print new message
    lcd.setCursor(0, 0); // set cursor to first column, second row
    lcd.print("CC3 Vel127 Ch2");
  }
// ================ End START Button ==================

// ==============Check if the TEMPO UP button is pressed =============
  if(tempo_up_button.isPressed()) {
    changeTempo(3); // Increase tempo by 3 BPM
  }

// ================TEMPO DOWN button is pressed ===========
  if(tempo_down_button.isPressed()) {
    changeTempo(-3); // Reduce tempo by 3 BPM
  }
  // ============ END TEMPO DOWN BUTTON =============

  // START button released
  if(start_button.isReleased()) {
    //Serial.println("The button 1 is RELEASED");
    digitalWrite(LED, LOW);
   // MIDI.sendNoteOff(60, 127, 1);
  }
    // TEMPO UP released
  if (tempo_up_button.isReleased()) {
    digitalWrite(LED, LOW);
   // MIDI.sendNoteOff(60, 127, 1);
  }
    // TEMPO DOWN released
  if (tempo_down_button.isReleased()) {
    digitalWrite(LED, LOW);
   // MIDI.sendNoteOff(60, 127, 1);
  }
}

