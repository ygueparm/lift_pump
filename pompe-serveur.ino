#define Moteur_pompe 2
#define Capteur_niveau_haut 23
#define Relais_securite 21
#define SIMULATEUR_CONTACTEUR 1
#if SIMULATEUR_CONTACTEUR
#define sortieContacteur 16
#endif
#define SCRIPT 1
#define MAX_SECURITE 3
#define TEMP_FONCTIONNEMENT_MOTEUR_config 2000
#define TEMP_ATTENTE_SECURITE 10000UL
#define MILLISECONDE 1
#define SECONDE (1000 * MILLISECONDE)
#define MINUTE (60 * SECONDE)
#define HEURE (60 * MINUTE)
#define JOUR (24 * HEURE)
#define MAX_ENTRIES 20
#define MAX_STRING_LENGTH 100
#define capteurContacteur 4

#define TEST   // laisser pour les test sur planche a pain
// #undef TEST // a activer pour la  production

#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>


// initialisation serveur AP

const char* ssid     = "Pompe_Relevage";
const char* password = "sidoniesidoniesidonie";
const char* apHostname = "pompe1.com"; // Le nom d'hôte que vous souhaitez utiliser

bool capteurBloque = false;
bool moteurEnMarche = false;
bool relaisDeclenche = false;
int securiteDeclanche = 0;
unsigned long tempsDebut;
unsigned long tempsRelaisDeclenche;
bool continuerProgramme = true; // Variable de contrôle
unsigned long tempsrelai ;
unsigned long tempsTotal = 0;
unsigned long jours, heures, minutes, secondes;
unsigned long dernierDemarrage = 0;
unsigned long TEMP_FONCTIONNEMENT_MOTEUR = TEMP_FONCTIONNEMENT_MOTEUR_config ;
unsigned long tempsDepuisDernierDemarrage[MAX_ENTRIES] = {0}; //2 tableau pour stocker les valeurs
unsigned long tempsReelDeFonctionnement[MAX_ENTRIES] = {0};   //2 tableau pour stocker les valeurs
int indiceDerniereEntreeTempsDepuis = 0;                //initialisation des 2 indice des 2 tableau pour stocker les valeurs
int indiceDerniereEntreeTempsReel = 0;                  //initialisation des 2 indice des 2 tableau pour stocker les valeurs

int nombreEntries = 0; // Compteur du nombre d'entrées dans le tableau
int indiceDerniereEntree = 0;
// calcul du temps reel de fonctionnement avec le retour du contacteur
volatile unsigned long tempsDebutReel = 0;
volatile unsigned long tempsFinReel = 0;
volatile bool contacteurDeclenche = false;
int simulateurContacteur = 0 ; //simulation retour contacteur
unsigned long tempsReel = 0; // variable du temps reel de fonctionnement
bool miseAJourEffectuee = false; // indicateur de mise a jout tableau
bool initialisationVoyant = false;

String contenuTableau;

DNSServer dnsServer;
AsyncWebServer server(80);


void setup() {
  Serial.begin(115200);
  Serial.println("\n[*] Creating AP");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password,13); // Utilisation du canal 13
  Serial.print("[+] AP Created with IP Gateway ");
  Serial.println(WiFi.softAPIP());

  // Configure le DNS local avec le serveur AP
  //**** affiche le tableau des dernier demarrages
  // Configure le DNS local avec le serveur AP
  dnsServer.start(53, apHostname, WiFi.softAPIP());

  //page d'accueuil avec le resumé de focntionnement




  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String htmlContent = "<!DOCTYPE html>\n";
    htmlContent += "<html>\n";
    htmlContent += "<head>\n";
    htmlContent += "<meta charset=\"UTF-8\">\n"; //  pour l'encodage UTF-8
    htmlContent += "<title>Pompe Relevage résumé</title>\n";
    htmlContent += "<style>\n";
    htmlContent += ".voyant {\n";
    htmlContent += "  width: 100px;\n";
    htmlContent += "  height: 100px;\n";
    htmlContent += "  border-radius: 100%;\n";
    htmlContent += "  margin-left: 300px;\n";
    htmlContent += "}\n";
    htmlContent += ".voyant-eteint {\n";
    htmlContent += "  background-color: gray;\n";
    htmlContent += "}\n";
    htmlContent += ".voyant-vert {\n";
    htmlContent += "  background-color: #00FF00;\n";
    htmlContent += "}\n";
    htmlContent += "label, input {";
    htmlContent += "  display: block;";
    htmlContent += "  margin-bottom: 40px;";
    htmlContent += "}";
    htmlContent += "input {";
    htmlContent += "  font-size: 40px;"; // Ajustez la taille de la police
    htmlContent += "  width: 150px;"; // Ajustez la largeur du champ de saisie
    htmlContent += "}";
    htmlContent += "button {";
    htmlContent += "  font-size: 40px;"; // Ajustez la taille de la police du bouton
    htmlContent += "  width: 150px;"; // Ajustez la largeur du bouton
    htmlContent += "}";
    htmlContent += "body {\n";
    htmlContent += "  margin: 0; \n";
    htmlContent += "}\n";

    htmlContent += "table, th, td {\n";
    htmlContent += "  border: 3px solid skyblue;\n";
    htmlContent += "  border-collapse: collapse;\n";
    htmlContent += "}\n";
    htmlContent += "caption, th, td {\n";
    htmlContent += "  padding: 8px;\n";
    htmlContent += "  font-size: 25px;\n";
    htmlContent += "}\n";
    htmlContent += "</style>\n";
    htmlContent += "</head>\n";
    htmlContent += "<body>\n";

    //message si la pompe a eu trop de securité declenché

    if (!continuerProgramme || capteurBloque ) {
      htmlContent += "<h2 style=\"font-size: 48px;\">Mise en sécurité de la pompe</h2>\n";

      // Affichage de l'information sur le capteur bloqué
      if (capteurBloque) {
        htmlContent += "<p style=\"font-size: 40px;\">Capteur bloqué : OUI</p>\n";
      } else {
        htmlContent += "<p style=\"font-size: 40px;\">Capteur bloqué : NON</p>\n";
      }

      // Affichage de l'information sur le trop d'alarme
      if (securiteDeclanche >= MAX_SECURITE) {
        htmlContent += "<p style=\"font-size: 40px;\">Trop de hors tolérance courant : OUI</p>\n";
      } else {
        htmlContent += "<p style=\"font-size: 40px;\">Trop de hors tolérance courant : NON</p>\n";
      }

      htmlContent += "<form method=\"POST\" action=\"/resume-program\">\n";
      htmlContent += "<button type=\"submit\" style=\"font-size: 24px;\">Reprendre le programme</button>\n";
      htmlContent += "</form>\n";
    }


    //suite page accueuil

    htmlContent += "<h1>Résumé fonctionnement <br> pompe relevage :</h1>";

    unsigned long tempsDepuisDernierFonctionnement = millis() - dernierDemarrage;
    unsigned long joursDepuisDernier = tempsDepuisDernierFonctionnement / JOUR;
    unsigned long heuresDepuisDernier = (tempsDepuisDernierFonctionnement % JOUR) / HEURE;
    unsigned long minutesDepuisDernier = ((tempsDepuisDernierFonctionnement % JOUR) % HEURE) / MINUTE;

    //champ pour modifier le temps de fonctionnement de la pompe
    htmlContent += "<form action='/set-time' method='GET'>";
    htmlContent += "<label for='new-time'><h3>Nouveau temps de fonctionnement (en secondes)</h3> :</label>";
    htmlContent += "<input type='number' id='new-time' name='new-time' required>";
    htmlContent += "<button type='submit'>Modifier</button>";
    htmlContent += "</form>";

    // tableau du temps de marche et de l'intervalle de demarrage

    htmlContent += "<h2>Temps depuis le dernier<br> fonctionnement : " + String(joursDepuisDernier) + "j " + String(heuresDepuisDernier) + "h " + String(minutesDepuisDernier) + "m</h2>";
    htmlContent += "<table>";
    htmlContent += "<caption></caption>";
    htmlContent += "<tr><th>Indice</th><th>Temps marche réel</th><th>intervale démarrage</th></tr>";

    // int indiceAffichage = indiceDerniereEntree;
    // int numeroIncrement = indiceDerniereEntreeTempsReel;

    int indiceAffichage = indiceDerniereEntreeTempsDepuis; // Utilisez l'indice des temps depuis le dernier démarrage
    int numeroIncrement = 1; // Commencez à compter à partir de 1


    for (int i = 0; i < MAX_ENTRIES; i++) {
      if (tempsReelDeFonctionnement[indiceAffichage] > 0 || tempsDepuisDernierDemarrage[indiceAffichage] > 0) {
        htmlContent += "<tr>";
        htmlContent += "<td>" + String(i) + "</td>"; // Ajoutez l'indice au début de la ligne
        // Ajoutez ici les colonnes pour le temps de fonctionnement réel et le temps depuis le dernier démarrage
        htmlContent += "<td>" + String(tempsReelDeFonctionnement[indiceAffichage]) + " secondes</td>\n";

        unsigned long joursAffichage = tempsDepuisDernierDemarrage[indiceAffichage] / JOUR;
        unsigned long heuresAffichage = (tempsDepuisDernierDemarrage[indiceAffichage] % JOUR) / HEURE;
        unsigned long minutesAffichage = ((tempsDepuisDernierDemarrage[indiceAffichage] % JOUR) % HEURE) / MINUTE;

        htmlContent += "<td>" + String(joursAffichage) + "j " + String(heuresAffichage) + ":" + (minutesAffichage < 10 ? "0" : "") + String(minutesAffichage) + "</td>\n";

        htmlContent += "</tr>";
        numeroIncrement++;
        if (numeroIncrement >= MAX_ENTRIES) {
          numeroIncrement = 0;
        }
      }
      indiceAffichage = (indiceAffichage + 1) % MAX_ENTRIES;
    }
    // code pour demarrer manuellement la pompe et l'arreter manuellement


    htmlContent += "<div id=\"voyant\" class=\"voyant voyant-eteint\"></div>"; // Voyant initial éteint

    htmlContent += "<button type=\"button\" onclick=\"demarrerPompe()\" style=\"width: 300px; height: 200px;\";\">Démarrer la pompe</button>\n";
    htmlContent += "<button type=\"button\" onclick=\"arreterPompe()\" style=\"width: 300px; height: 200px;\"\">Arrêter la pompe</button>\n";



#if SCRIPT
    htmlContent += "<script>\n";
    htmlContent += "function demarrerPompe() {\n";
    htmlContent += "  document.getElementById(\"voyant\").className = \"voyant voyant-vert\";\n";
    htmlContent += "  envoyerRequete(\"/start-pump\", \"POST\");\n";
    htmlContent += "}\n";
    htmlContent += "function arreterPompe() {\n";
    htmlContent += "  document.getElementById(\"voyant\").className = \"voyant voyant-eteint\";\n";
    htmlContent += "  envoyerRequete(\"/stop-pump\", \"POST\");\n";
    htmlContent += "}\n";
    htmlContent += "function envoyerRequete(url, method) {\n";
    htmlContent += "  var xhr = new XMLHttpRequest();\n";
    htmlContent += "  xhr.open(method, url, true);\n";
    htmlContent += "  xhr.setRequestHeader(\"Content-Type\", \"application/x-www-form-urlencoded\");\n";
    htmlContent += "  xhr.send();\n";
    htmlContent += "}\n";
    htmlContent += "</script>\n";
#endif

    request->send(200, "text/html", htmlContent);
  });

  server.on("/start-pump", HTTP_POST, [](AsyncWebServerRequest * request) {
    // Code pour démarrer la pompe
    digitalWrite(Moteur_pompe, HIGH);

    request->send(200, "text/html", "<script>window.location.replace('/');</script>"); //retour a la page principal
  });

  server.on("/stop-pump", HTTP_POST, [](AsyncWebServerRequest * request) {
    // Code pour arrêter la pompe avec le bouton
    digitalWrite(Moteur_pompe, LOW);
    request->send(200, "text/html", "<script>window.location.replace('/');</script>");   //retour a la page principal
  });


  //partie pour modifier le temps de fcontionnement de la pompe en secondes
  server.on("/set-time", HTTP_GET, [](AsyncWebServerRequest * request) {
    String newTimeStr = request->arg("new-time");
    if (newTimeStr.length() > 0) {
      unsigned long newTime = newTimeStr.toInt();
      // Utilisez la valeur de newTime pour modifier le temps de fonctionnement de la pompe
      TEMP_FONCTIONNEMENT_MOTEUR = newTime * 1000; // Convertit en millisecondes
    }
    String responseContent = "<!DOCTYPE html>\n";
    responseContent += "<html>\n";
    responseContent += "<head>\n";
    responseContent += "<meta charset=\"UTF-8\">\n";
    responseContent += "<style>\n";
    responseContent += "body {\n";
    responseContent += "  font-size: 45px;\n"; // Ajustez la taille de la police selon vos préférences
    responseContent += "}\n";
    responseContent += "</style>\n";
    responseContent += "</head>\n";
    responseContent += "<body>\n";
    responseContent += "<p>Nouveau temps de fonctionnement enregistré : " + newTimeStr + " secondes</p>";
    responseContent += "<p><a href=\"/\">Retour</a></p>\n";
    responseContent += "</body></html>";
    request->send(200, "text/html", responseContent);
  });

  //relance le processus sur appuie bouton après la mise securité suite trop de declenchement relais securité

  server.on("/resume-program", HTTP_POST, [](AsyncWebServerRequest * request) {
    continuerProgramme = true; // Inverse la valeur de continuerProgramme
    securiteDeclanche = 0; // remise a 0 du nombre de declenchement du relais de securité
    capteurBloque = false ; //force le capteur a 0 pour retenter un redemarrage
    String responseContent = "<!DOCTYPE html>\n";
    responseContent += "<html>\n";
    responseContent += "<head>\n";
    responseContent += "<meta charset=\"UTF-8\">\n";
    responseContent += "</head>\n";
    responseContent += "<body>\n";
    responseContent += "<p style=\"font-size: 24px;\">Programme repris !</p>\n";
    responseContent += "<p style=\"font-size: 24px;\"><a href=\"/\">Retour</a></p>\n"; // Ajout du lien de retour
    responseContent += "</body></html>";

    request->send(200, "text/html", responseContent);
  });
  ;

  //lancement du serveur
  server.begin();


#ifdef TEST

  pinMode(Moteur_pompe, OUTPUT);
  pinMode(Capteur_niveau_haut, INPUT_PULLDOWN); // 
  pinMode(Relais_securite, INPUT_PULLUP); // 
  pinMode(capteurContacteur, INPUT_PULLUP); //pour calculer le temps de contact du contacteur 

#endif 

#ifndef TEST

  pinMode(Moteur_pompe, OUTPUT);
  pinMode(Capteur_niveau_haut, INPUT_PULLDOWN); // 
  pinMode(Relais_securite, INPUT_PULLDOWN); // 
  pinMode(capteurContacteur, INPUT_PULLDOWN); //pour calculer le temps de contact du contacteur 

#endif


#if SIMULATEUR_CONTACTEUR
  pinMode(sortieContacteur, OUTPUT);
#endif

  // Initialisation des variables
  moteurEnMarche = false;
  securiteDeclanche = 0;
  continuerProgramme = true; // Initialise la variable de contrôle




}


void loop() {

  dnsServer.processNextRequest(); //pour permettre au serveur DNS de traiter les requêtes entrantes de manière continue.

  // Sort de la boucle loop pour empecher le redemarrage de la pompe
  if (!continuerProgramme || capteurBloque) {
    //on s'assure que le moteur est coupé

    digitalWrite(Moteur_pompe, LOW);
    return;
  }


  int etatContacteur = digitalRead(capteurContacteur);
  int niveauHaut = digitalRead(Capteur_niveau_haut);
  int etatRelaisSecurite = digitalRead(Relais_securite);
  unsigned long tempsActuel = millis();

  // verification si il y a eu trop de declenchement relais securité
  if (securiteDeclanche >= MAX_SECURITE) {
    continuerProgramme = false; // Arrête le programme dans loop
    Serial.println("Arrêt en raison de declenchements consecutif de la securité.");
    return;
  }



  //Démarrage du moteur de la pompe

  if (niveauHaut == HIGH && !relaisDeclenche && securiteDeclanche < MAX_SECURITE && !capteurBloque) {
    if (!moteurEnMarche) {
      moteurEnMarche = true;

      digitalWrite(Moteur_pompe, HIGH);

#if SIMULATEUR_CONTACTEUR
      digitalWrite(sortieContacteur, HIGH);
#endif

      tempsDebut = tempsActuel;
      tempsRelaisDeclenche = 0;  // Réinitialise le temps de déclenchement du relais de securité
      tempsReel = 0; //Mise a 0 de la variable qui donne le temps de fonctionnement réel
      relaisDeclenche = false;   // Réinitialise l'état du relais déclenché
      Serial.println("Démarrage du moteur.");

    }
  } else {
    if (moteurEnMarche) {



      // gestion securité lorsque le moteur fonctionne
      if (etatRelaisSecurite == HIGH) {
        digitalWrite(Moteur_pompe, LOW);


#if SIMULATEUR_CONTACTEUR
        digitalWrite(sortieContacteur, LOW);
#endif

        moteurEnMarche = false;
        relaisDeclenche = true;
        tempsRelaisDeclenche = tempsActuel;  // Enregistre le moment du déclenchement
        Serial.println("Relais de sécurité déclenché.");

      }



      //arret automatique après un certain temps

      else if (tempsActuel - tempsDebut >= TEMP_FONCTIONNEMENT_MOTEUR ) {
        moteurEnMarche = false;

        digitalWrite(Moteur_pompe, LOW);

#if SIMULATEUR_CONTACTEUR
        digitalWrite(sortieContacteur, LOW);
#endif

        Serial.println("Arrêt automatique du moteur après 10 secondes.");




        tempsTotal = tempsActuel;

        //delai depuis dernier demarrage moteur sur moniteur serie

        unsigned long tempsEcoule_1 = tempsActuel - dernierDemarrage;
        jours = tempsEcoule_1 / JOUR;
        heures = (tempsEcoule_1 % JOUR) / HEURE;
        minutes = ((tempsEcoule_1 % JOUR) % HEURE) / MINUTE;
        Serial.print("Dernier démarrage il y a ");
        Serial.print(jours);
        Serial.print("j ");
        Serial.print(heures);
        Serial.print(":");
        if (minutes < 10) {
          Serial.print("0");
        }
        Serial.println(minutes);
        Serial.print("il y a en cours ");
        Serial.print(securiteDeclanche);
        Serial.println(" securité declenché");


        //remplissage du tableau avec les données

        if (tempsDebut != 0) {
          tempsDepuisDernierDemarrage[indiceDerniereEntreeTempsDepuis] = tempsEcoule_1;

          // Mise à jour des indices pour chaque tableau

          indiceDerniereEntreeTempsDepuis = (indiceDerniereEntreeTempsDepuis + 1) % MAX_ENTRIES;
          indiceDerniereEntreeTempsReel = (indiceDerniereEntreeTempsReel + 1) % MAX_ENTRIES;

          Serial.print("indiceDerniereEntreeTempsReeltemps reel de fonctionnement :");
          Serial.println(indiceDerniereEntreeTempsReel);
          Serial.print("indiceDerniereEntreeTempsDepuis temps depuisdernier demarrage : ");
          Serial.println(indiceDerniereEntreeTempsDepuis);


          miseAJourEffectuee = false; // Réinitialiser l'indicateur
        }


        dernierDemarrage = tempsActuel; // Met à jour le temps du dernier démarrage


      }
    }
    // autorisation de demarrer après la mise en securité du moteur si le temps d'attente est ecoulé

    if (!moteurEnMarche) {
      if (relaisDeclenche && tempsActuel - tempsRelaisDeclenche >= TEMP_ATTENTE_SECURITE) {
        relaisDeclenche = false;
        securiteDeclanche++;

        //     Serial.print("il y a en cours :");
        //    Serial.println(securiteDeclanche);


        tempsDebut = tempsActuel;  // Réinitialise le temps de démarrage

        Serial.println("Temps d'attente après le d&eacuteclenchement du relais terminé. Le moteur peut être redémarré.");
      }
    }
  }

  //calcul du temps reel de fcontionnement

  if (tempsDebut != 0) {
    if (etatContacteur == HIGH && !contacteurDeclenche) {
      tempsDebutReel = millis();
      contacteurDeclenche = true;
    } else if (etatContacteur == LOW && contacteurDeclenche) {
      tempsFinReel = millis();
    tempsReel = tempsFinReel - tempsDebutReel;

      tempsReel = (tempsFinReel - tempsDebutReel) / 1000; // Conversion en secondes

      Serial.print("Temps de fonctionnement réel : ");
      Serial.print(tempsReel);
      Serial.println(" secondes  ");

      // Met à jour le temps réel dans la même entrée du tableau
      tempsReelDeFonctionnement[indiceDerniereEntreeTempsReel] = tempsReel;

      miseAJourEffectuee = true; //valide la mise a jour du tableau
      contacteurDeclenche = false;
    }
  }

  //verification capteur bloqué

  if (moteurEnMarche && digitalRead(niveauHaut) == HIGH && (millis() - tempsDebut) >= (TEMP_FONCTIONNEMENT_MOTEUR))
  {
    capteurBloque = true;
  }


}
