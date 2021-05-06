#include <avr/pgmspace.h>
#include <pitches.h>

#define NOTE_REST 0

#define TONE_PIN 8

// The music tempo in beats per minute.
#define TEMPO 180

// Note durations in milliseconds for the given tempo.
#define QUAVER    (60000/(TEMPO * 2))
#define CROTCHET  (2 * QUAVER)
#define MINIM     (2 * CROTCHET)
#define SEMIBREVE (2 * MINIM)

struct Note
{
  short pitch;
  short duration;
};


// Jingle Bells, arrangement (C) 2013 by Kyle Coughlin
//
// Taken from https://www.christmasmusicsongs.com/jingle-bells-sheet-music.html
const Note g_jingleBells[] PROGMEM =
{
  // Verse - Bar 1
  { NOTE_D4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  { NOTE_G4, CROTCHET },
  // Verse - Bar 2
  { NOTE_D4, MINIM+CROTCHET },
  { NOTE_D4, QUAVER },
  { NOTE_D4, QUAVER },
  // Verse - Bar 3
  { NOTE_D4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  { NOTE_G4, CROTCHET },
  // Verse - Bar 4
  { NOTE_E4, SEMIBREVE },
  // Verse - Bar 5,
  { NOTE_E4, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  // Verse - Bar 6
  { NOTE_FS4, SEMIBREVE},
  // Verse - Bar 7
  { NOTE_D5, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_A4, CROTCHET },
  // Verse - Bar 8
  { NOTE_B4, SEMIBREVE },
  // Verse Bar - 9
  { NOTE_D4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  { NOTE_G4, CROTCHET },
  // Verse - Bar 10
  { NOTE_D4, SEMIBREVE },
  // Verse - Bar 11
  { NOTE_D4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  { NOTE_G4, CROTCHET },
  // Verse - Bar 12
  { NOTE_E4, MINIM+CROTCHET },
  { NOTE_E4, CROTCHET },
  // Verse - Bar 13
  { NOTE_E4, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  // Verse - Bar 14
  { NOTE_D5, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_D5, CROTCHET },
  // Verse - Bar 15
  { NOTE_E5, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_A4, CROTCHET },
  // Verse - Bar 16
  { NOTE_G4, MINIM },
  { NOTE_REST, MINIM },
  // Chorus - Bar 1
  { NOTE_B4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, MINIM },
  // Chorus - Bar 2
  { NOTE_B4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, MINIM },
  // Chorus - Bar 3
  { NOTE_B4, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_G4, CROTCHET+QUAVER },
  { NOTE_A4, QUAVER},
  // Chorus - Bar 4
  { NOTE_B4, SEMIBREVE },
  // Chorus - Bar 5
  { NOTE_C5, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_C5, CROTCHET+QUAVER },
  { NOTE_C5, QUAVER },
  // Chorus - Bar 6
  { NOTE_C5, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, QUAVER },
  { NOTE_B4, QUAVER },
  // Chorus - Bar 7
  { NOTE_B4, CROTCHET },
  { NOTE_A4, CROTCHET },
  { NOTE_A4, CROTCHET },
  { NOTE_B4, CROTCHET },
  // Chorus - Bar 8
  { NOTE_A4, MINIM },
  { NOTE_D5, MINIM },
  // Chorus - Bar 9
  { NOTE_B4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, MINIM },
  // Chorus - Bar 10
  { NOTE_B4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, MINIM },
  // Chorus - Bar 11
  { NOTE_B4, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_G4, CROTCHET+QUAVER },
  { NOTE_A4, QUAVER},
  // Chorus - Bar 12
  { NOTE_B4, SEMIBREVE },
  // Chorus - Bar 13
  { NOTE_C5, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_C5, CROTCHET+QUAVER },
  { NOTE_C5, QUAVER },
  // Chorus - Bar 14
  { NOTE_C5, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, CROTCHET },
  { NOTE_B4, QUAVER },
  { NOTE_B4, QUAVER },
  // Chorus - Bar 15
  { NOTE_D5, CROTCHET },
  { NOTE_D5, CROTCHET },
  { NOTE_C5, CROTCHET },
  { NOTE_A4, CROTCHET },
  // Chorus - Bar 16
  { NOTE_G4, SEMIBREVE },
  // Rest at the end
  { NOTE_REST, SEMIBREVE }
};

#define JINGLE_BELLS_LENGTH (sizeof(g_jingleBells)/sizeof(g_jingleBells[0]))

#define LED_RED   9
#define LED_GREEN 6
#define LED_BLUE  5

unsigned long g_currentTime = 0;
unsigned long g_nextNoteStartTime = 0;
unsigned long g_currentNote = 0;
unsigned long g_nextToggleLedTime = 0;
bool g_ledState = false;

static void playNextNote();
static void toggleLed();

void setup()
{
    playNextNote();
    toggleLed();
}

void loop()
{
  g_currentTime = millis();

  if (g_currentTime >= g_nextNoteStartTime)
  {
    playNextNote();
  }
  
  if (g_currentTime >= g_nextToggleLedTime)
  {
    toggleLed();
  }
}

void playNextNote()
{
  const Note* currentNote = &g_jingleBells[g_currentNote++ % JINGLE_BELLS_LENGTH];
  short currentPitch = pgm_read_word(&currentNote->pitch);
  unsigned long currentDuration = pgm_read_word(&currentNote->duration);

  if (currentPitch != NOTE_REST)
  {
    tone(TONE_PIN, currentPitch, ((currentDuration * 9) / 10));
  }
  else
  {
    noTone(TONE_PIN);
  }

  g_nextNoteStartTime = g_currentTime + currentDuration;
}

void toggleLed()
{
  analogWrite(LED_RED, g_ledState ? 0 : 255);
  analogWrite(LED_GREEN, g_ledState ? 255 : 0);
  analogWrite(LED_BLUE, 0);

  g_ledState = !g_ledState;
  g_nextToggleLedTime = g_currentTime + CROTCHET;
}
