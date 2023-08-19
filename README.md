# Lift Pump - Summary of Operation
en francais à la fin

When I bought my house, there was an original pump, but after many breakdowns and having noticed the presence of mud and H2S in large quantities, I embarked on this project. The cable sheathing was being eaten away by the H2S and the pump body, which was not submerged, was very badly attacked by rust.

 I switched to this esp32 management system, as I wasn't satisfied with what was available. Too expensive, no configuration possible. It was useful once when hair started to surround the rotating joint of the stator, I was able to repair before it was too late, and it's never glamorous this kind of unexpected failure. and it's very instructive this kind of project. I have an arduino in production with an RPI for the wifi part, I'm going to switch everything over to an ESP32, here's my project. I'm not a programmer by trade, so forgive my mistakes and monstrosities.

This project consists of creating a control system for a lift pump. The aim is to monitor the operation of the pump, record the actual operating times, the intervals between starts and display this information in tabular form via a web interface. and also :
- limit the presence of H2S gas and odours
- make full use of tank capacity
- protect the pump well in advance from existing problems, hair on the rotor, blocked rotor, blocked sensor and avoid burning out the motor.

<img src="https://github.com/ygueparm/lift_pump/blob/main/screenshot.jpg" alt="screen shoot" width="300">

## Features

- Automatic pump start-up when the high level sensor is activated.
- Automatic pump shutdown after a certain operating time.
- Recording of actual running times and intervals between starts.
- Web interface for displaying the latest pump operations.
- Safety device to detect any blockage of the high level sensor.


<img src="https://github.com/ygueparm/lift_pump/blob/main/schema_pomperelevage.png" alt="pompe relevage " width="500">

## Components used

- Microcontroller: ESP32
- High level sensor: Stainless steel float switch, liquid water level sensor, double ball float switch, reed switch.
- Safety relay: Relay to cut power to the pump if necessary, type :
    K8DT-AW2CD Current monitoring relay
Omron

RM35JA32MW Current monitoring relay, 
schneider electrique

GRI8-03/04 On current or under current
GEYA
- Web interface: Use of AsyncWebServer to display the table of the last operations, in AP mode and possibility of configuring the pumping times with a telephone.

## Operation

1. The ESP32 microcontroller continuously monitors the status of the high level sensor.
2. When the high level sensor is activated, the pump starts automatically.
3. The pump runs for a set time (e.g. 10 seconds) before stopping automatically. to be configured in the web interface
4. Actual running times and intervals between starts are recorded in tables. accessible by telephone
5. A web interface allows the last pump operations to be displayed in table form.
6. If the high level sensor is blocked after start-up, the safety relay is activated to cut power to the pump.
until reset by the user via the web interface.
7. connect to the pompe_relevage wifi, enter the name: pomperelevage in the address bar to access the site.

## Configuration

- Make sure you configure the appropriate constants in the code, such as operating times and pins used.
- Make sure you have the necessary libraries, such as WiFi, DNSServer and AsyncWebServer. to be able to compile
- I have chosen to put everything in a single file to simplify compilation for novices with the arduino-1.8.19 gui.

## Note

This project is designed for educational and experimental purposes. Make sure you take all the necessary precautions when handling electrical equipment.

# Pompe de Relevage - Résumé de Fonctionnement

lorsque j'ai acheté ma maison, il y avait une pompe de relevage, à force de panne et après avoir constaté la présence de boue et de H2S en forte quantité, je me suis lancé dans ce projet. La gaine des cables etait rongé par l'H2S et le corps de pompe qui n'était pas submergé très attaqué par de la rouille.

 je suis passé à cette gestion par esp32, je n'etais pas satisfait de ce qui existait. trop chère , pas de configuration possible. Cela a servi une fois lorsque des cheveux ont commencé a entourer le joint tournant du stator, j'ai pu depanner avant que cela soit trop tard, et ce n'est jamais glamour ce genre de panne non prévu. et c'est très instructif ce genre de projet. J'ai un arduino en production avec un RPI pour la partie wifi, je vais basculé tout cela sur un ESP32, voici mon projet. je ne suis pas programmeur de metier, pardonné mes erreur et monstruosité. j'ai fait le choix de n'avoir qu'un seul fichier pour qu'un debutant puisse le compiler sans peine

Ce projet consiste en la création d'un système de contrôle pour une pompe de relevage. L'objectif est de surveiller le fonctionnement de la pompe, enregistrer les temps de fonctionnement réel, les intervalles entre les démarrages et afficher ces informations sous forme de tableau via une interface web. et aussi :
- de limité la presence de gaz H2S et d'odeur
- d'utiliser pleinement la capacité du reservoir
- de proteger la pompe très en amont des problemes existant, cheveux sur le rotor, rotor bloqué, capteur bloqué et de ne pas griller son moteur
<img src="https://github.com/ygueparm/lift_pump/blob/main/screenshot.jpg" alt="screen shoot" width="300">

## Fonctionnalités

- Démarrage automatique de la pompe lorsque le capteur de niveau haut est activé.
- Arrêt automatique de la pompe après un certain temps de fonctionnement.
- Enregistrement des temps de fonctionnement réel et des intervalles entre les démarrages.
- Interface web permettant d'afficher les derniers fonctionnements de la pompe.
- Mise en place d'une sécurité pour détecter un blocage éventuel du capteur de niveau haut.

## Composants Utilisés

- Microcontrôleur : ESP32
- Capteur de niveau haut : Interrupteur à flotteur en acier inoxydable, capteur de niveau d'eau liquide, interrupteur à flotteur à Double bille, reed
- Relais de sécurité : Relais pour couper l'alimentation de la pompe en cas de besoin, de type :
    K8DT-AW2CD Relais de surveillance de courant
Omron

RM35JA32MW Relais de surveillance de courant, 
schneider electrique

GRI8-03/04 Sur courant ou sous courant
GEYA
- Interface Web : Utilisation d'AsyncWebServer pour afficher le tableau des derniers fonctionnements, en mode AP et possibilité de configurer les temps de pompage avec un téléphone

## Fonctionnement

1. Le microcontrôleur ESP32 surveille en permanence l'état du capteur de niveau haut.
2. Lorsque le capteur de niveau haut est activé  la pompe démarre automatiquement.
3. La pompe fonctionne pendant un temps défini (par exemple, 10 secondes) avant de s'arrêter automatiquement. a configurer dans l'interface web
4. Les temps de fonctionnement réel et les intervalles entre les démarrages sont enregistrés dans des tableaux. accessible par telephone
5. Une interface web permet d'afficher les derniers fonctionnements de la pompe sous forme de tableau.
6. En cas de blocage du capteur de niveau haut après le démarrage, le relais de sécurité est activé pour couper l'alimentation de la pompe.
jusqu'a un rearmement de l'utilisateur par l'interface web
7. se connecter au wifi pompe_relevage , entrer le nom : pomperelevage dans la barre d'adresse pour acceder au site

## Configuration

- Assurez-vous de configurer les constantes appropriées dans le code, telles que les temps de fonctionnement et les broches utilisées.
- Assurez-vous de disposer des bibliothèques nécessaires, telles que WiFi, DNSServer et AsyncWebServer. pour pouvoir compiler
- j'ai fait le choix de tout mettre dans un seul fichier pour simplifier la compilation pour des novices avec la gui arduino-1.8.19

## Remarque

Ce projet est conçu à des fins éducatives et expérimentales. Assurez-vous de prendre toutes les précautions nécessaires lors de la manipulation d'appareils électriques.

