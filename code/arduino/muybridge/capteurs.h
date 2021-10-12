/*
  Conversations avec Muybridge
  Code d'origine : Michel Glaize
  Modifications : Rémi Sarrailh (µsini)
  **********************************************
  **** 👀 Gestion des capteurs LIDAR Garmin ***
  **********************************************
  📝 Code : https://github.com/garmin/LIDARLite_Arduino_Library
  📕 Manuel : https://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf
*/

/* ⏲ Gestion du temps */
unsigned long derniereContreMesure = 0; // pour le délai entre mesure et contre-mesure
unsigned long temps_Absence = 0;

// 🎞 On valide une étape si elle est actif 5 fois
void verification_Etape(int etapeActuel) {
  tableauEtape[compteur_Etape] = etapeActuel;
  compteur_Etape++;

  if(compteur_Etape > 5) {
    for(int i = 1; i < compteur_Etape; i++) {
      Serial.print("[👀] Vérification Etape:");
      Serial.println(String(tableauEtape[i]));
      if(tableauEtape[i -1] == tableauEtape[i]) {
          integrite_Etape++;
      }
    }
    if(integrite_Etape == 5) {
      nouvelle_Etape = true;
      etape = tableauEtape[0];
      Serial.print("[👀] Changement d'étape validée : ");
      Serial.print(etape_Precedente);
      Serial.print(" ---> ");
      Serial.println(etape);
      if(depart_Parcours == 0) {
        if(etape != -1) {
          Serial.println("[👀] Utilisateur detecté, Activation du moteur et Démarrage du compteur d'absence...");
          envoi_Par_OSC(IP_iPad, port_Ipad, adresse_Etat_Parcours, "Attendez votre tour !");
          depart_Parcours = 1;
          digitalWrite(MOTEUR_ACTIF, LOW);
        }
      } else {
        if(etape == -1) {
          if(etape_Precedente != -1){
            temps_Absence = millis();
          }
        }
      }
    } else {
      Serial.print("Changement d'étape trop brusque - ");
      Serial.println(integrite_Etape);
    }
    integrite_Etape = 0;
    compteur_Etape = 0;
  }
}

// 📏 --> 🎟 Converti une distance en cm en étape de 50 pas
void conversion_distance_vers_etape(long distance) {
  //Serial.println("Distance en cours");

  // 🚧 Test sur Parcours Absent
  // temporisationEtape(-1);

  int etapeActuel = 0;

  // 🚧 Test retour de la distance
  ///Serial.println(String(distance));

  // Si la distance est inférieure à la distance minimum, afficher la première étape
  if(distance < distanceMinimum) {
    etapeActuel = 0;
  // Si la distance est supérieur à la distance maximum, on considére que l'utilisateur
  // est sorti de l'installation
  } else if(distance >= distance_Maximum) {
    etapeActuel = -1;
  }
  // Sinon on calcule quel étape affichée (coupé en distance intermédiaire en retirant la distance mimimal)
  // Afin de ne pas inciter l'utilisateur de se rapprocher trop du capteur.
  else {
    // Serial.println(String((distance - distanceMinimum) / distanceIntermediaire));
    etapeActuel = (distance - distanceMinimum) / distance_Intermediaire;
  }
  // Serial.println("Fin du calcul");
  // Serial.println(String(etapeActuel));

  if(etapeActuel >= 49) {
    etapeActuel = 49;
  }

  verification_Etape(etapeActuel);
}

void test_Capteurs() {
  unsigned long distance1 = 0;
  unsigned long distance2 = 0;
  distance1 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_1, HIGH);
  distance2 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_2, HIGH);
  Serial.println("[👀] TEST Capteurs");
  distance1 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_1, HIGH);
  distance2 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_2, HIGH);
  Serial.println(" ----> " + String(distance1));
  Serial.println(" ----> " + String(distance2));
  distance1 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_1, HIGH);
  distance2 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_2, HIGH);
  Serial.println(" ----> " + String(distance1));
  Serial.println(" ----> " + String(distance2));
  delay(1000);
}

// Récupère la distance de deux capteurs et l'envoi au convertisseur
// de distance en étape
void recuperation_Position_Utilisateur() {

    unsigned long distance1 = 0;
    unsigned long distance2 = 0;

    //Serial.println("Position en cours de calcul");
    // Récupération de la distance à l'aide de méthode PWM
    distance1 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_1, HIGH);
    distance2 = pulseIn(MONITEUR_CAPTEUR_DISTANCE_2, HIGH);
    //Serial.println("Position en cours de calcul");

    // Si la distance est supérieur à 0, la convertir en cm
    if (distance1 != 0) {
      distance1 = distance1 / 10;
    }
    if (distance2 != 0) {
      distance2 = distance2 / 10;
    }


    //Serial.println("1-" + String(distance1));
    //Serial.println("2-" + String(distance2));


    // On pars du principe que si un capteur est supérieur à la distance maximale
    // Alors soit le capteur ne détecte pas la personne soit personne n'est présent
    if ((distance1 >= distance_Maximum) && (distance2 >= distance_Maximum)) {
      conversion_distance_vers_etape(distance1);
    } else {
      // Si la distance du capteur 1 est inférieure à la distance Maximum, la récupérer
      if (distance1 <= distance_Maximum) {
        conversion_distance_vers_etape(distance1);
      } else {
        // Sinon retourner la distance du capteur 2
        conversion_distance_vers_etape(distance2);
      }
    }
}



/*
***************************************
**** 👀 Paramètrages des capteurs  ***
***************************************
*/

void parametrages_Capteurs() {
  Serial.println("[👀] Activation des capteurs LIDAR Garmin");

  // Les capteurs LIDAR ont une broche qui une fois active lance la détection de la distance
  pinMode(ACTIVATION_CAPTEURS_DISTANCES, OUTPUT);
  digitalWrite(ACTIVATION_CAPTEURS_DISTANCES, LOW);

  // On utilise les capteurs LIDAR en PWM
  pinMode(MONITEUR_CAPTEUR_DISTANCE_1, INPUT);
  pinMode(MONITEUR_CAPTEUR_DISTANCE_2, INPUT);

  //test_Capteurs();
}
