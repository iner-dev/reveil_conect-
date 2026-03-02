#include "song_runner.h"

// Définition de l'objet global
DFRobotDFPlayerMini myDFPlayer;

// Le parachute : une variable pour savoir si on peut utiliser le module sans crash
bool audioReady = false;

song_runner::song_runner() {
    t0 = 0;
}

void song_setup() {
    // Initialisation du port série 2 (Pins 16 et 17 comme dans votre test)
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    
    Serial.println(F("--- Initialisation Audio ---"));
    
    // Délai de sécurité pour stabiliser l'alimentation
    delay(2000); 

    // Tentative d'initialisation
    if (!myDFPlayer.begin(Serial2, true, true)) {
        Serial.println(F("ERREUR : Le DFPlayer ne répond pas. Le parachute est ACTIF."));
        audioReady = false; 
        return; 
    }

    // Si on arrive ici, c'est que le module a répondu
    audioReady = true;
    myDFPlayer.volume(SONG_VOLUME);
    Serial.println(F("Audio prêt et sécurisé !"));
}

void song_runner::start() {
    if (!audioReady) return; // Sécurité : on ne fait rien si pas initialisé

    myDFPlayer.loopFolder(01);
    t0 = millis();
    Serial.println(F("Lecture lancée."));
}

int song_runner::Secret_Mode(){
    if (!audioReady) return 0; // Sécurité
    myDFPlayer.volume(SECRET_VOLUME);
    myDFPlayer.advertise(1);
}
int song_runner::End_Secret_Mode(){
    if (!audioReady) return 0; // Sécurité
    myDFPlayer.stopAdvertise();
    myDFPlayer.volume(SONG_VOLUME);
}


int song_runner::run() {
    if (!audioReady) return 0; // Sécurité

    // Exemple de logique pour arrêter après un certain temps (MAX_PLAY_TIME)
    // Assurez-vous que MAX_PLAY_TIME est défini dans vos paramètres
    if (t0 > 0 && (millis() - t0 > MAX_PLAY_TIME)) { 
        stop();
    }
    /*
    if (myDFPlayer.available()) {
      printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
    }/**/
    
    return 0; // Toujours renvoyer une valeur pour éviter les erreurs de lien
}

void song_runner::stop() {
    if (!audioReady) return; // Sécurité

    myDFPlayer.stop(); // .stop() est souvent plus stable que .sleep()
    t0 = 0;
    Serial.println(F("Lecture arrêtée."));
}

bool song_runner::is_active() {
    if (!audioReady) return false; // Sécurité : évite d'interroger un module absent

    // On vérifie l'état. Si le module renvoie une erreur (-1), on retourne false.
    int s = myDFPlayer.readState();
    return (s == 1); 
}


void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  
}
