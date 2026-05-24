#include <LiquidCrystal_I2C.h>
#include <Servo.h>
//LiquidCrystal lcd(4,5,13,12,11,10);
LiquidCrystal_I2C lcd(0x27,16,2);

//initialisation
float distance_fixe =20;//la distance est en cm
float hauteur = 37.00;//La hauteur de notre poubelle en cm

//Detection exterieure
int trigpin1=2 , echopin1 = 3; //declaration des broches de la detection externe
float distance_ext;

//Detection interieure //declaration des broches de la detection interne
int trigpin2 =4 , echopin2 = 5;
float distance_in, niveau_poubelle;

//sevo_moteur_d'ouverture et fermeture
Servo Moteur;
int Moteur_pin = 6;
int debut_pas =68;
int led_signalr = 7, led_signalv = 9; //la led rouge signalant la fermeture et quand c'est remplir
int son_signal = 8; //la sonnerie de signalisation
int position_servo;
//Conditionnement Proprietaire
int boutton=10;
void setup() {
  // put your setup code here, to run once:
pinMode(trigpin1,OUTPUT);
pinMode(trigpin2,OUTPUT);

pinMode(echopin1,INPUT);
pinMode(echopin2,INPUT);
pinMode(boutton,INPUT);

pinMode(led_signalr,OUTPUT);
pinMode(son_signal,OUTPUT);
pinMode(led_signalv,OUTPUT);

lcd.init();
lcd.backlight();

Moteur.attach(Moteur_pin);
Serial.begin(9600);
Moteur.write(debut_pas);
}

void loop() {
  // put your main code here, to run repeatedly:
int bouton_etat = digitalRead(boutton);
  
distance_ext = detection_object(); // calcule de la distance exterieure 
niveau_poubelle =  detection_niveau();
distance_in = hauteur - niveau_poubelle;
Serial.println(distance_in);

// Quand le servomoteur est au repos on affiche l'etat de la pubelle 
int p = Moteur.read(); //position du servo moteur
if (p == debut_pas)
{
  affiche_niveau();
}
if ((distance_ext <= distance_fixe) and (distance_ext != 0.00) and(distance_ext >0))
{
  if ((distance_in <= hauteur-15) and (distance_in != 0) and (distance_in >0))
  {
    action_mot(debut_pas);
    delay(100);
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("Ouverture...");
    delay(4000);
    //Moteur.detach();
    distance_ext = detection_object();
    //Moteur.attach(Moteur_pin);
    arret_mot();
  
  }
  
   if ((distance_in >= hauteur-15) and (p == debut_pas))
  { //Ici on clignote 3 fois pour signaler que la poubelle est pleine 
    for(int i=1;i<=4; i++)
    {
      signal();
      lcd.clear();
      lcd.print("Poubelle pleine");
    } 
  }  
} 

 if (distance_in > hauteur-15 and bouton_etat==1) 
    {
      action_mot(debut_pas);
      delay(20000);
        arret_mot();
    }
      
}
  





//renvoie la distance de la Detection externe
float  detection_object()
{
long duration1;
float distance;
digitalWrite(trigpin1,HIGH);
delay(5);
digitalWrite(trigpin1,LOW);
duration1 = pulseIn(echopin1,HIGH,10000L);
distance = (duration1*0.034)/2 ;                //Calculer la distance d'objet ou dechet en cm
return distance;             //retourne la distance detectée
//Serial.println(distance_ext);
}

//Detection interne de niveau
float detection_niveau()
{
  long duration2;
float distance;
digitalWrite(trigpin2,HIGH);
delay(100);
digitalWrite(trigpin2,LOW);
duration2 = pulseIn(echopin2,HIGH);
distance = (duration2*0.034)/2 ;                //Calculer la distance du niveau de poubelle  en cm

return distance;     
}

//Signalisation si la poubelle atteint la hauteur maximale(pleine)
  void signal(){
    digitalWrite(led_signalr,HIGH);
    digitalWrite(son_signal,HIGH);
    delay(500);
    digitalWrite(led_signalr,LOW);
    digitalWrite(son_signal,LOW);
    delay(100);
    
  }


//Mettre le servo-moteur en action pour ouvrir la poubelle
void action_mot(int debut)
{
  signal_o();
  for(int pas= debut; pas>=0; pas--)
  {
    Moteur.write(pas); 
  }
}

//Mettre le servo-moteur en action pour fermer la poubelle
void arret_mot()
{ 
  lcd.clear();
  lcd.setCursor(1,1);
  lcd.print("Fermeture...");
  for(int pas=0; pas<=debut_pas; pas++)
  {
    Moteur.write(pas);
    delay(10);
  }
  lcd.clear();
}

//Definition et Affichage de l'etat de la poubelle 
void affiche_niveau()
{
  niveau_poubelle =  detection_niveau();
 distance_in = hauteur - niveau_poubelle;
 
  if (distance_in>=22)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Niveau Poubelle:"); 
    lcd.setCursor(3,1);
    lcd.print( "Pleine");
    delay(200);
  }
  else if (distance_in >=17 and distance_in <=22)
    {
     lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Niveau Poubelle:"); 
    lcd.setCursor(0,1);
    lcd.println("Pr.esque Pleine"); 
        delay (300);

    }
  else if (distance_in >=12 and distance_in <=17)
  {
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Niveau Poubelle:"); 
    lcd.setCursor(0,1);
    lcd.println("A moitier");
    delay(400);
    }
    
    else if (distance_in >=7 and distance_in <= 12)
    {
       lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Niveau Poubelle:"); 
    lcd.setCursor(0,1);
    lcd.println("Presque Moitier");
        delay (300);
    }
    else if (distance_in >=0 and distance_in <= 8)
    {
       lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Niveau Poubelle:"); 
    lcd.setCursor(4,1);
    lcd.println("VIDE");
        delay (500);
    }
    else
    {
      lcd.clear();
    lcd.setCursor(0,0);
    lcd.println("Niveau Poubelle:"); 
    lcd.setCursor(3,1);
    lcd.println(distance_in);
        delay (300);

    }
    
  }
  
  void signal_o()
  {
    digitalWrite(led_signalv,HIGH);
    digitalWrite(son_signal,HIGH);
    delay(50);
    digitalWrite(led_signalv,LOW);
    digitalWrite(son_signal,LOW);
    
  }

  
