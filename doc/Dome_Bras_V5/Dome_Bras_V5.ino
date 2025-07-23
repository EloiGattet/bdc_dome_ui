//DOME BRAS PAR ELOI GATTET
//MERCURIO
//Refonte du code complète, ajout de paramètres, cleanup
//Tentative de calcul dynamique de position de Leds
//Carte Bras V5
//Ajout de quicksort
//21/06/2021


//######### Bibliothèques

#include <LiquidCrystal595.h>
#include <AccelStepper.h>
#include <Rotary.h>
#include <EEPROM.h>

#include "param.h"

//######### Naming pinout
#define Bouton 8
#define OutA 7
#define OutB 2
#define opto3 A4
#define opto4 A5
#define LedR 13
#define LedG 12
#define LedB 11
#define Buzzer 6
#define step_mot A0
#define dir_mot A2
#define Enable A3
#define lcddata 5
#define lcdlatch 3
#define lcdclock 4
#define latchPin 9
#define clockPin A1
#define dataPin 10

//##### Test pour pwm over shift register (afficher toutes les leds en meme temps)
/*
  #define ShiftRegisterPWM_LATCH_PORT PORTB
  #define ShiftRegisterPWM_LATCH_MASK 0B00000010 //0B00000010
  #define ShiftRegisterPWM_CLOCK_PORT PORTC
  #define ShiftRegisterPWM_CLOCK_MASK 0B00000010
  #define ShiftRegisterPWM_DATA_PORT PORTB
  #define ShiftRegisterPWM_DATA_MASK 0B00000100
  #include "ShiftRegisterPWM.h"
  ShiftRegisterPWM sr(2, 255);
*/


//Constantes système
#define LCDwidth 16
#define LCDheight 2
bool updatelcdMenu = true;
bool updatelcd = true;

//Tableau de leds
uint8_t tabled[] = {128, 64, 8, 16, 32, 4, 2, 1};
uint8_t tabled2[] = { 2, 4, 32, 16, 8, 64, 128, 1};

//Adresse pour la sauvegarde EEPROM
uint8_t adr_tps_pose = 0;
uint8_t adr_nb_photos = 2;
uint8_t adr_speed = 4;
uint8_t adr_acc = 6;
uint8_t adr_tps_stab = 8;
uint8_t adr_nbpdv = 10;



//uint16_t stepspartour = 13334;
uint16_t stepspartour = 13500;
uint8_t nbpdv = 32;


// Parametres de prise de vue pour changement manuel
uint16_t tps_pose = 1000;
uint8_t nb_photos = 1;
uint16_t tps_stab = 500;
uint8_t stepspeed = 200;
uint8_t stepacc = 200;

bool StopPDV = false;

// parameter_ parametre(type, val, step, min, max, name)
parameter_ p_nbpdv("int", nbpdv, 1, 2, 64, "NB DE PDV");
parameter_ p_tps_pose("int", tps_pose, 50, 0, 20000, "TPS DE PDV");
parameter_ p_tps_stab("int", tps_stab, 50, 0, 20000, "TPS STAB");
//parameter_ p_nb_photos("int", nb_photos, 1, 1, 5, "NB IMG/PDV");
parameter_ p_speed("int", stepspeed, 50, 0, 5000, "VIT MOT");
parameter_ p_acc("int", stepacc, 50, 0, 10000, "ACC MOT");



//Paramètres internes de gestion des menus
uint8_t Menu = 4;
uint8_t SubMenu = 0;

//parameter_* Liste_Reglages[] = {&p_nbpdv, &p_tps_pose, &p_tps_stab, &p_nb_photos, &p_speed, &p_acc};
parameter_* Liste_Reglages[] = {&p_nbpdv, &p_tps_pose, &p_tps_stab, &p_speed, &p_acc};

//######### Création des objets du programme

LiquidCrystal595 lcd(lcddata, lcdlatch, lcdclock);
Rotary rotary = Rotary(OutB, OutA);
AccelStepper stepper(1, step_mot, dir_mot);



typedef struct ledAndAngle {
  uint8_t led;
  uint8_t angle;
};

ledAndAngle *tableau ;


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ######################################     SETUP     #################################
// ######################################               #################################
// ######################################               #################################
//#######################################################################################




void setup()
{
  //On charge les paramètres en mémoire
  ReadParameters();
  //commenter Read et décommenter Save pour changer les valeurs via arduino ide
  //SaveParameters();
  // Initialisation input

  pinMode(OutA, INPUT);
  pinMode(OutB, INPUT);
  pinMode(Bouton, INPUT);
  // Initialisation output
  pinMode(Enable, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(opto3, OUTPUT);
  pinMode(opto4, OUTPUT);
  pinMode(LedR, OUTPUT);
  pinMode(LedG, OUTPUT);
  pinMode(LedB, OUTPUT);
  pinMode(lcddata, OUTPUT);
  pinMode(lcdlatch, OUTPUT);
  pinMode(lcdclock, OUTPUT);
  // Enable en haut pour désactiver le moteur
  digitalWrite(Enable, HIGH);

  //########## Début du programme

  lcd.begin(16, 2);
  rotary.begin(false);
  lcd.setLED2Pin(HIGH);
  lcd.setLED1Pin(HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  DOME BRAS V5");
  lcd.setCursor(0, 1);
  lcd.print("   PDV OPTIM");
  beep();
  delay(500);
  lcd.clear();
  stepper.setMaxSpeed(p_speed._val); //set max speed the motor will turn (steps/second)
  stepper.setAcceleration(p_acc._val); //set accX (steps/second^2)
  stepper.setSpeed(0);
  stepper.setCurrentPosition(0);
  stepper.moveTo(0);
}


// #######################################################################################
// ######################################                #################################
// ######################################                #################################
// ###################################### MENU PRINCIPAL #################################
// ######################################                #################################
// ######################################                #################################
// #######################################################################################


void loop()
{

  digitalWrite(Enable, HIGH);

  if (updatelcdMenu == true) {
    zero();
    lcd.clear();
    lcd.setCursor(0, 0);
    switch (Menu)
    {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("<1  REGLAGES   >");
        lcd.setCursor(5, 1);
        lcd.print("ENTRER");
        break;
      case 1:
        lcd.print("<2 TEST MOTEUR >");
        lcd.setCursor(5, 1);
        lcd.print("ENTRER");
        break;
      case 2:
        lcd.print("<3  TEST LUM   >");
        lcd.setCursor(6, 1);
        lcd.print("Click");
        break;
      case 3:
        lcd.print("<4 PDV PHOTO   >");
        lcd.setCursor(0, 1);
        lcd.print(Liste_Reglages[1]->_val);
        lcd.print("ms | ");
        lcd.print(int(nbpdv * 15 / 2));
        lcd.print("imgs");
        break;
      case 4:
        lcd.print("<5 PDV OPTIM  >");
        lcd.setCursor(0, 1);
        lcd.print(Liste_Reglages[1]->_val);
        lcd.print("ms | ");
        lcd.print(int(nbpdv * 35));
        lcd.print("imgs");
        break;
    }

    updatelcdMenu = false;
  }

  if (rotary.process() == DIR_CW) {
    Menu ++;
    if (Menu > 4) {
      Menu = 0;
    };
    updatelcdMenu = true;

  }
  else if (rotary.process() == DIR_CCW)
  {

    if (Menu == 0 ) {
      Menu = 4;
    }
    else {
      Menu --;
    }
    updatelcdMenu = true;

  }

  if (!digitalRead(Bouton))
  {
    delay(250);
    updatelcdMenu = true;
    switch (Menu)
    {
      case 0:
        menureglages();
        break;
      case 1:
        testmot();
        break;
      case 2:
        testlum();
        break;
      case 3:
        pdv();
        break;
      case 4:
        pdv210img();
        break;
    }
  }
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ######################################    REGLAGES   #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################


void menureglages()
{
  boolean sortie = false;
  updatelcd = true;
  int SubMenu = 0;
  int numElements = (sizeof(Liste_Reglages) / sizeof(Liste_Reglages[0]));
  ReadParameters();
  while (sortie == false)
  {
    if (updatelcd == true) {
      if (SubMenu == numElements) {
        Menu_Retour();
      }
      else {
        Liste_Reglages[SubMenu]->afficher(lcd);
      }
      updatelcd = false;
    }

    if (rotary.process() == DIR_CW) {
      SubMenu++;
      if (SubMenu > numElements) {
        SubMenu = 0;
      }
      updatelcd = true;
    }
    if (rotary.process() == DIR_CCW) {
      SubMenu--;
      if (SubMenu < 0) {
        SubMenu = numElements;
      };
      updatelcd = true;
    }

    if (digitalRead(Bouton) == 0) {
      delay(200);
      if (SubMenu == numElements) {
        SaveParameters();
        sortie = true;
      }
      else {
        Liste_Reglages[SubMenu]->ctrl(lcd, rotary);
      }
      updatelcd = true;
    }
  }
}



// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ######################################    TEST LUM   #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################


void  testlum()
{ digitalWrite(Enable, HIGH);
  boolean updatelcd = true;
  boolean sortie = false;
  int i = 0;
  int j = 0;
  int k = 0;

  singA();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<Test  Lumieres>");

  while (sortie == false)
  { unsigned char result = rotary.process();
    int boutonState = digitalRead(Bouton);
    if (result == DIR_CW) {
      updatelcd = true;
      i++;
      if (i > 16) {
        i = 0;
      };

    } else if (result == DIR_CCW) {
      updatelcd = true;
      i--;
      if (i < 0) {
        i = 16;
      };

    }

    digitalWrite(LedB, 0);
    if (boutonState == 0)
    {
      if (i == 16)
      {
        ALLLEDS();
        /*sr.interrupt(ShiftRegisterPWM::UpdateFrequency::Medium);
          for (uint8_t i = 0; i < 8; i++) {
          //uint8_t val = (uint8_t)(((float)sin(millis() / 150.0 + i / 8.0 * 2.0 * PI) + 1) * 128);
          sr.set(i, 32);
          }*/
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Photo Flat (3s)");
        delay(3000);
        delay(50);
        digitalWrite(opto3, HIGH);
        delay(2);
        digitalWrite(opto4, HIGH);
        delay(100);
        digitalWrite(opto3, LOW);
        digitalWrite(opto4, LOW);
        delay(tps_pose);
      }
      else {
        //blabla
      }
      zero();
      updatelcd = true;
      sortie = true;
      delay(300);
    }
    if (updatelcd == true)
    { lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("<Test  Lumieres>");
      if (i == 16)
      {
        lcd.setCursor(4, 1);
        lcd.print("ALL");
        ALLLEDS();
        singB();
      }
      else
      {
        lcd.setCursor(4, 1);
        lcd.print("Led : ");
        lcd.print(i + 1);
        LED2(i);
        singB();
      }
      updatelcd = false;
    }

  }

  zero();
  updatelcdMenu = true;
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ######################################    TEST MOT   #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################

void  testmot()
{

  digitalWrite(Enable, HIGH);
  stepper.setMaxSpeed(p_speed._val); //set max speed the motor will turn (steps/second)
  stepper.setAcceleration(p_acc._val); //set accX (steps/second^2)
  stepper.setSpeed(0);
  stepper.setCurrentPosition(0);
  singA();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tour de manege!");
  digitalWrite(Enable, LOW);
  DeplaceMoteurAbs(&stepper, stepspartour);
  singA();
  digitalWrite(Enable, HIGH);
  stepper.setCurrentPosition(0);
  stepper.moveTo(0);
}

// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### PRISE DE VUES #################################
// ######################################               #################################
// ######################################               #################################
// ######################################################################################
void pdv()
{
  int i = 0;
  int j = 0;
  int start = 0;
  unsigned long previousMillis = millis();
  bool sortie = false;
  bool StopPDV = false;
  int increment = stepspartour / nbpdv ;
  updatelcd = true;
  // LANCEMENT DE LA PRISE DE VUE
  zero();
  lcd.clear();
  digitalWrite(LedR, 0);
  digitalWrite(LedG, 0);
  digitalWrite(LedB, 1);
  stepper.setMaxSpeed(p_speed._val); //set max speed the motor will turn (steps/second)
  stepper.setAcceleration(p_acc._val); //set accX (steps/second^2)
  stepper.setCurrentPosition(0);
  singA();
  digitalWrite(LedB, 0);
  // ON BLOQUE LE BRAS JUSQUA LA FIN DE LA PDV
  digitalWrite(Enable, LOW);
  Click();
  while (i < nbpdv && sortie == false && StopPDV == false)
  {
    if (updatelcd == true)
    {
      lcd.clear();
      lcd.setCursor(0, 0) ;
      lcd.print(F("Etapes restantes"));
      lcd.setCursor(0, 1);
      lcd.print(nbpdv - i);
      updatelcd = false;
    }
    if (i % 2) start = 0;
    else start = 1;
    DeplaceMoteurAbs(&stepper, increment * i);
    digitalWrite(LedB, 0);
    singB();
    //######ON STABILISE
    previousMillis = millis();
    LED2(start);
    delay(tps_stab);
    /*while (millis() - previousMillis >= tps_stab && sortie == false && StopPDV == false)
      {
      if (!digitalRead(Bouton)) //Check erreur
      {
        zero();
        digitalWrite(LedR, 1);
        sortie = true;
        StopPDV = true;
      }
      }*/
    singA();
    previousMillis = millis();
    //########ON VERIFIE SI PAIR OU IMPAIR

    //###### ON ALLUME ET ON DECLENCHE
    for (int led = start; led <= 14; led = led + 2)
    {
      //On allume les leds une par une
      LED2(led);
      j = 0;
      //delay(10);
      //      while (j < p_nb_photos._val)
      //    {
      Click();
      updatelcd = true;
      delay(tps_pose);
      //    j++;
      // }
    }
    digitalWrite(LedB, 1);
    i++;
  }

  // ON BLOQUE LE BRAS JUSQUA LA FIN DE LA PDV
  digitalWrite(Enable, HIGH);
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### PRISE DE VUES #################################
// ######################################       210     #################################
// ######################################               #################################
// ######################################################################################
void pdv210img()
{
  int angle = 0;
  int tailleTableau = nbpdv * 35;
  int increment = stepspartour / 360 ;
  tableau = new ledAndAngle[tailleTableau];

  lcd.clear();
  lcd.setCursor(0, 0) ;
  lcd.print("CALCUL POSITIONS");
  CreerTableauLeds(nbpdv);
  //delay(500);
  lcd.clear();
  lcd.setCursor(0, 0) ;
  lcd.print("OPTIM  POSITIONS");
  TrierTableau();
  //delay(500);
  //Debut de la prise de vue:

  // ON BLOQUE LE BRAS JUSQUA LA FIN DE LA PDV
  digitalWrite(Enable, LOW);


  LED2(1);
  Click();
  lcd.clear();
  int previousangle = 0;
  lcd.setCursor(0, 0) ;
  lcd.print("LANCEMENT PDV");
  delay(1000);
  singA();
  lcd.setCursor(0, 0) ;
  lcd.print("RESTE:");
  for (int x = 0; x < tailleTableau; x++)
  {

    angle = map(tableau[x].angle, 0, 255, 0, 360);
    lcd.clear();
    lcd.setCursor(0, 0) ;
    lcd.print("RESTE:");
    lcd.print(tailleTableau - x);
    lcd.print(" imgs");
    lcd.setCursor(0, 1);
    lcd.print("angle:");
    lcd.print(angle);
    lcd.print(" led:");
    lcd.print(tableau[x].led);
    if (angle != previousangle)
    {
      digitalWrite(LedB, 1);
      DeplaceMoteurAbs(&stepper, increment * angle);
      //######ON STABILISE
      digitalWrite(LedR, 1);
      delay(tps_stab);
      digitalWrite(LedB, 0);
      digitalWrite(LedR, 0);
      previousangle = angle;
    }
    //On allume les leds une par une
    LED2(tableau[x].led - 1);
    int j = 0;
    delay(10);
    //    while (j < p_nb_photos._val)
    //  {
    Click();
    delay(tps_pose);
    //   j++;
    //  }
  }
  Mario();
  zero();
  lcd.clear();
  lcd.setCursor(0, 0) ;
  lcd.print("FIN PDV");

  lcd.setCursor(0, 1) ;
  lcd.print("RETOUR ORIGINE");
  DeplaceMoteurAbs(&stepper, 0);

  // ON BLOQUE LE BRAS JUSQUA LA FIN DE LA PDV
  digitalWrite(Enable, HIGH);
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### AUTRES FCTS   #################################
// ######################################               #################################
// ######################################               #################################
//#########################################################################################

//##### FONCTION CAMERA

void Click()
{
  digitalWrite(opto3, LOW);
  digitalWrite(opto4, LOW);
  digitalWrite(LedG, 1);
  delay(5);
  digitalWrite(opto3, HIGH);
  delay(10);
  digitalWrite(opto4, HIGH);
  delay(25);
  digitalWrite(opto3, LOW);
  digitalWrite(opto4, LOW);
  digitalWrite(LedG, 0);

}
//###FONCTIONS MOTEUR

bool DeplaceMoteurAbs(AccelStepper* Moteur, long Pos)
{
  bool sortie = false;
  bool StopPDV = false;
  Moteur->moveTo(Pos);
  while (sortie == false)
  {
    if ( Moteur->distanceToGo() == 0) sortie = true;
    if (digitalRead(Bouton) == 0)
    {
      Moteur->stop();
      StopPDV = true;
    }
    Moteur->run();
  }
  sortie = false;
}



//####FONCTIONS LEDS

void LED2(int ledindex)
{ //noInterrupts();
  //sr.interrupt(ShiftRegisterPWM::UpdateFrequency::Stop);
  digitalWrite(latchPin, LOW);
  int bras = ledindex / 8;
  int led = 0;
  for (int k = bras + 1; k < 2; k++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
  }
  if (ledindex < 8) {
    led = tabled[ledindex % 8];
  }
  else {
    led = tabled2[ledindex % 8];
  }

  shiftOut(dataPin, clockPin, MSBFIRST, led);

  for (int j = 0; j < bras; j++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
  }
  digitalWrite(latchPin, HIGH);
  // interrupts();
}

void ALLLEDS()
{
  zero();
  /*
    digitalWrite(latchPin, LOW);

    for (int k = 1; k < 9; k++)
    {
    shiftOut(dataPin, clockPin, MSBFIRST, B10000000);
    }
    digitalWrite(latchPin, HIGH);
  */
  //sr.interrupt(ShiftRegisterPWM::UpdateFrequency::Medium);
  for (uint8_t i = 0; i < 8; i++)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, B11111111);
    //uint8_t val = (uint8_t)(((float)sin(millis() / 150.0 + i / 8.0 * 2.0 * PI) + 1) * 128);
    //sr.set(i, 10);
  }
}

void zero()
{
  digitalWrite(latchPin, LOW);
  for (int i = 0; i < 2; i++)
  {
    shiftOut(dataPin, clockPin, LSBFIRST, B00000000);
  }
  digitalWrite(latchPin, HIGH);
}

//####FONCTIONS BEEPER
void Mario()
{
  tone(Buzzer, 1976);
  delay(100);
  tone(Buzzer, 2637);
  delay(500);
  noTone(Buzzer);
}

void beep()
{
  tone(Buzzer, 2000);
  delay(100);
  noTone(Buzzer);
}

void singA() {
  tone(Buzzer, 1000);
  delay(250);
  noTone(Buzzer);
}

void singB() {
  tone(Buzzer, 1600);
  delay(30);
  noTone(Buzzer);
}


void Menu_Retour()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("<    RETOUR    >");
}



// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### EEPROM        #################################
// ######################################               #################################
// ######################################               #################################
//#######################################################################################

void SaveParameters()
{

  EEPROMWriteInt(adr_nbpdv, p_nbpdv._val);
  EEPROMWriteInt(adr_tps_pose, p_tps_pose._val);
  EEPROMWriteInt(adr_tps_stab, p_tps_stab._val);
  //  EEPROMWriteInt(adr_nb_photos, p_nb_photos._val);
  EEPROMWriteInt(adr_speed, p_speed._val );
  EEPROMWriteInt(adr_acc, p_acc._val);
  tps_pose = p_tps_pose._val;
  tps_stab = p_tps_stab._val;
  //  nb_photos = p_nb_photos._val;
  nbpdv = p_nbpdv._val;
  stepper.setMaxSpeed(p_speed._val); //set max speed the motor will turn (steps/second)
  stepper.setAcceleration(p_acc._val); //set acc (steps/second^2)
}

void ReadParameters()
{
  p_nbpdv._val = EEPROMReadInt(adr_nbpdv);
  p_tps_stab._val = EEPROMReadInt(adr_tps_stab);
  p_tps_pose._val = EEPROMReadInt(adr_tps_pose);
  //  p_nb_photos._val = EEPROMReadInt(adr_nb_photos);
  p_speed._val = EEPROMReadInt(adr_speed);
  p_acc._val = EEPROMReadInt(adr_acc);
  tps_pose = p_tps_pose._val;
  tps_stab = p_tps_stab._val;
  //  nb_photos = p_nb_photos._val;
  nbpdv = p_nbpdv._val;
  stepper.setMaxSpeed(p_speed._val); //set max speed the motor will turn (steps/second)
  stepper.setAcceleration(p_acc._val); //set acc (steps/second^2)

}

void EEPROMWriteInt(int address, int value)
{
  byte two = (value & 0xFF);
  byte one = ((value >> 8) & 0xFF);

  EEPROM.update(address, two);
  EEPROM.update(address + 1, one);
}

int EEPROMReadInt(int address)
{
  long two = EEPROM.read(address);
  long one = EEPROM.read(address + 1);

  return ((two << 0) & 0xFFFFFF) + ((one << 8) & 0xFFFFFFFF);
}


// ######################################################################################
// ######################################               #################################
// ######################################               #################################
// ###################################### TABLEAUX      #################################
// ######################################               #################################
// ######################################               #################################
//#######################################################################################

void CreerTableauLeds(uint8_t nbleds)
{
  int cpteurtableau = 0;
  int taboffset[] = {0, 3, 1, 4, 2};
  for (int colonne = 1; colonne <= 3; colonne++)
  {
    float angleincrement = 255.00 / (nbleds * bit(colonne - 1));
    for (int indexled = 1; indexled <= 5; indexled++)
    {
      float offset = taboffset[indexled - 1] * 255 / (nbleds * bit(colonne - 1) * 5);
      for (int k = 1; k <= nbleds * bit(colonne - 1); k++)
      {
        float angle =  angleincrement * (k - 1) + offset;
        int led = indexled + ((colonne - 1) * 5);
        int angleint = (int) angle;
        uint8_t leduint = (uint8_t) led;
        uint8_t angleuint = (uint8_t) angleint;
        tableau[cpteurtableau].led = leduint;
        tableau[cpteurtableau].angle = angleuint ;
        cpteurtableau ++;
      }
    }
  }
}


/* function to sort arr using shellSort */
void TrierTableau()
{
  uint8_t tempangle, templed;
  uint16_t n = nbpdv * 35;
  // Start with a big gap, then reduce the gap
  for (int gap = n / 2; gap > 0; gap /= 2)
  {
    // Do a gapped insertion sort for this gap size.
    // The first gap elements a[0..gap-1] are already in gapped order
    // keep adding one more element until the entire array is
    // gap sorted
    for (int i = gap; i < n; i += 1)
    {
      // add a[i] to the elements that have been gap sorted
      // save a[i] in temp and make a hole at position i
      tempangle = tableau[i].angle;
      templed = tableau[i].led;
      // shift earlier gap-sorted elements up until the correct
      // location for a[i] is found
      int j;
      for (j = i; j >= gap && tableau[j - gap].angle > tempangle; j -= gap)
      {
        tableau[j].angle = tableau[j - gap].angle;
        tableau[j].led = tableau[j - gap].led;
      }
      //  put temp (the original a[i]) in its correct location
      tableau[j].angle = tempangle;
      tableau[j].led = templed;
    }
  }
  return 0;
}

/*
  void TrierTableau() //BuubleSort
  {
  uint8_t tempangle, templed;
  uint16_t cpteurtableau = nbpdv * 35;
  for (int i = 1; i < cpteurtableau; i++) {
    for (int j = 0; j < cpteurtableau - 1; j++) {
      //Serial.println("Tri de n:" + String(j + 1));
      if (tableau[j].angle > tableau[j + 1].angle) {
        //Serial.println("on monte la position " + String(j) + " car " + String(tableau[j].angle) + " >" + String(tableau[j + 1].angle));
        tempangle = tableau[j].angle;
        templed = tableau[j].led;
        tableau[j].angle = tableau[j + 1].angle;
        tableau[j].led = tableau[j + 1].led;
        tableau[j + 1].angle = tempangle;
        tableau[j + 1].led = templed;
      }
    }
  }
  //Serial.println("Tableau trié");
  }*/
