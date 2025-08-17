#include <Arduino.h>
#include <string.h>

#define BAUD 28800

#define LED_ROSU 11
#define LED_VERDE 10
#define LED_ALBASTRU 9

#define BUTON_START 2
#define BUTON_DIFICULTATE 3

#define BACKSPACE 8


unsigned long timpTastare[3] = {5000, 4000, 3000}, timpJoc = 33000, momentSelectareCuvant = 0, timpDebounce = 300, timpIncepere = 3000, eroareIncepere = 50;
char cuvant[30], cuvantTinta[30];
char dictionar [10][30] = {"robotica","arduino","mitocondrie","alfabet","portocaliu","elefant","vehicul","visator","numeros","paradis"};
int index = 0, dificultate = 0, stareLed = 0, clipiri = 0, scor = 0;
char numeDificultati[3][10] = {"\nUsor\n", "\nMediu\n", "\nGreu\n"};
volatile unsigned long momentApasareDificultate = 0, momentApasareStart = 0, momentIncepereJoc = 0, momentUltimaClipire = 0;
bool repaus = true, rulare = false;

void setRGB(int valRosu, int valVerde, int valAlbastru) 
{
  analogWrite(LED_ROSU, valRosu);
  analogWrite(LED_VERDE, valVerde);
  analogWrite(LED_ALBASTRU, valAlbastru);
}

int verificaCuvant(const char *cuvantDeVerificat)
{
  int lungime = strlen(cuvantDeVerificat);
  for(int i=0; i<lungime; i++)
  {
    if(cuvantDeVerificat[i] != cuvantTinta[i])
      return -1;
  }
  if(lungime == int(strlen(cuvantTinta)))
    return 1;
  return 0;
}

void finalizare()
{
  Serial.println("\nTerminat!\nScor:");
  Serial.println(scor);
  Serial.println("\n");
  setRGB(100, 100, 100);
}

void verificaStare()
{
  if(rulare && millis() - momentIncepereJoc > timpJoc)
  {
    repaus = true;
    rulare = false;
    finalizare();
  }
}

void incepere()
{
  if(millis() - momentUltimaClipire > timpIncepere/6)
  {
    momentUltimaClipire = millis();
    setRGB(100 * stareLed, 100 * stareLed, 100 * stareLed);
    stareLed = (stareLed + 1) % 2;

    if(!(clipiri % 2))
    {
      Serial.println(char(3 - clipiri/2 + 48));
    }

    clipiri++;
  }

  if(clipiri == 6)
  {
    index = 0;
    strcpy(cuvant, "");
    strcpy(cuvantTinta,dictionar[(random(10) + momentIncepereJoc) % 10]);
    Serial.println(cuvantTinta);
    momentSelectareCuvant = millis();
    setRGB(0, 100, 0);
    clipiri++;
  }
}

void startJoc()
{
  momentIncepereJoc = millis();
  scor = 0;

  momentUltimaClipire = millis();
  clipiri = 0;
}

void verificareButonDificultate()
{
  if(repaus)
  {
    if(millis() - momentApasareDificultate > timpDebounce && digitalRead(BUTON_DIFICULTATE) == LOW)
    {
      momentApasareDificultate = millis();
      dificultate = (dificultate + 1) % 3;
      Serial.println(numeDificultati[dificultate]);
    }
  }
}

void verificareButonStart()
{
  if(millis() - momentApasareStart > timpDebounce && digitalRead(BUTON_START) == LOW)
  {
    momentApasareStart = millis();
    repaus = !repaus;
    rulare = !rulare;
    if(rulare)
    {
      startJoc();
    }else
    {
      finalizare();
    }
  }
}

void citireLitera()
{
  char litera = Serial.read();
  if(int(litera) == BACKSPACE)
  {
    if(index > 0)
    {
      cuvant[index - 1] = NULL;
      index--;
    }
  }else
  {
    cuvant[index] = litera;
    index++;
    cuvant[index] = NULL;
  }
}

void alegereCuvantNou(int verdict)
{
  if(verdict)
  {
    scor++;
    Serial.println("\n");
  }else
  {
    Serial.println("\nTimp expirat!\n");
  }
  strcpy(cuvant," ");
  index = 0;
  strcpy(cuvantTinta,dictionar[(random(10) + momentIncepereJoc) % 10]);
  Serial.println(cuvantTinta);
  momentSelectareCuvant = millis();
}

void setup()
{
    Serial.begin(BAUD);

    pinMode(LED_ROSU, OUTPUT);
    pinMode(LED_VERDE, OUTPUT);
    pinMode(LED_ALBASTRU, OUTPUT);

    setRGB(100, 100, 100);

    pinMode(BUTON_START, INPUT_PULLUP);
    pinMode(BUTON_DIFICULTATE, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(BUTON_START), verificareButonStart, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTON_DIFICULTATE), verificareButonDificultate, FALLING);

    Serial.println("\nSetup complet\n");
}

void loop()
{
  verificaStare();

  if(rulare)
  {
    if(millis() - momentIncepereJoc <= timpIncepere + eroareIncepere)
    {
      incepere();
    }else
    {
      if(Serial.available()) 
      {
          citireLitera();
          
          if(verificaCuvant(cuvant) == 1)
          {
            alegereCuvantNou(1);
          }else if(verificaCuvant(cuvant) == -1)
          {
            setRGB(100, 0, 0);
          }else
            setRGB(0, 100, 0);
      }
      if(millis() - momentSelectareCuvant > timpTastare[dificultate])
      {
        alegereCuvantNou(0);
      }
    }
  }
}