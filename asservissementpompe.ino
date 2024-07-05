//bon ca marche :)
//declaration de variable
//reflechir a surveiller la tension d'alim des relais t

int CapteurNiveau = 2;  //entrée capteur niveau
int Pompe = 13 ;  //sortie commande moteur
int Protection = 7; //entré relais protection
int AlarmeCapteurBloque = 8 ; //sortie a voir si pas branché directement sur le pi
int AlarmeProtection = 9 ; //sortie pareil a branché direct sur le pi 
int EtatProtection = 0;
int EtatCapteurNiveau = 0 ;
int CompteurProtection = 1 ;
int TempsProtection = 0 ;
int VariableCompteur = 0;
int MemoireProtection = 0 ;
int CompteurActif = 0 ;
int CompteurActifTest = 0 ;




// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
/*  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(CapteurNiveau, INPUT);
  pinMode(Protection, INPUT);
  pinMode(AlarmeCapteurBloque, OUTPUT);
  pinMode(AlarmeProtection, OUTPUT);
  pinMode(Pompe, OUTPUT);
  digitalWrite(AlarmeCapteurBloque, LOW);
  digitalWrite(AlarmeProtection, LOW);
  */
}

// the loop routine runs over and over again forever:
void loop() {
  // puis on lit les broche:
  EtatProtection = digitalRead(Protection);
  EtatCapteurNiveau =  digitalRead(CapteurNiveau);

  do
  {
    EtatProtection = digitalRead(Protection);
    EtatCapteurNiveau =  digitalRead(CapteurNiveau);
    CompteurProtection = 1 ;
    delay(1000); //a supprimer c'est pour le debug

    //si le capteur est bloqué on eteind la pompe qd même et on allume une alarme
    if (CompteurActif == 1) {
      //on eteind la pompe
      digitalWrite(Pompe, LOW);

      //on allume une alarme capteur bloqué a l'etat haut + envoie de mail etc ...
      digitalWrite(AlarmeCapteurBloque, HIGH);

    }

    if ( EtatCapteurNiveau == 1 && EtatProtection != 1 && CompteurActif != 1) {
      //mise en route moteur si le capteur n'est pas bloqué et si a protection n'est pas active

      digitalWrite(Pompe, HIGH);
      CompteurActif = 1 ;
      for (int i = 0; i < 35000 ; i++)  //temps de fonctionnement total de la pompe 

      {
        EtatProtection = digitalRead(Protection);
        delay(1);
        if (EtatProtection == 1) {  //pendant le fonctionnement on verifie qu'il n'y a pas la protection
          digitalWrite(Pompe, LOW);
          break;
        }

      }
      
    }

    if (EtatCapteurNiveau == 0 ) {
      digitalWrite(Pompe, LOW); //pas
      Serial.println(CompteurActifTest);
      CompteurActif = 0 ;
      Serial.println("EtatCapteurNiveau");
      Serial.println(EtatCapteurNiveau);
    }

  } while (EtatProtection == 0 );

  //  on attend en multipliant le temps par le nombre de declenchement de protection de 10 minute de base

  do
  {
    /*   Serial.println("compteur protection :");
       Serial.println(CompteurProtection);
       Serial.println("protection à");
       Serial.println(EtatProtection);
    */
    EtatCapteurNiveau =  digitalRead(CapteurNiveau);
    EtatProtection = digitalRead(Protection);
    CompteurProtection++ ;

//temps d'attente avant de relancer le processus * 10 minutes a chaque declenchement de protection
    for (int i = 0; i < CompteurProtection * 1000 ; i++)
    {
      
      digitalWrite(Pompe, LOW);
      delay(1);
    }

// au bout d'un moment après 10 protection a l'etat haut on force 5 seconde le moteur pour tenté de le debloqué
    if (CompteurProtection == 9) {
      digitalWrite(Pompe, HIGH);
      digitalWrite(AlarmeProtection, HIGH);
      delay(5000);
      digitalWrite(Pompe, LOW);
      
       }

    //la on bloque tout et on mets la remontée d'alarme


    if (CompteurProtection > 10) {
      VariableCompteur = 0 ;
      digitalWrite(AlarmeProtection, HIGH);
      // puis attendre looongtemps
      delay(10000) ;
    }
  }
  while (EtatProtection == 1);

}
