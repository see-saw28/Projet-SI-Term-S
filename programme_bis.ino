
#include <SoftwareSerial.h>
#include <Wire.h>
#include "ITG3200.h"

ITG3200 gyro;
SoftwareSerial bluetooth(10, 11);
/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>


// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the backlight color
int WhichScreen = 1;
int n=0;		//déclaration de toutes les variables utilisées
int angle=0;
int roulette=0;
int step=200;
int level=1;
int l=0;
int p=0;
float delta=0;
float debut=0;
float delta1=0;
int end=0;
float a=0;
float t=0;
float debut2;
int cm=0;
int quit=0;
bool rmv=false;
String message[11];

byte Char1[] = {B00011,B00000,B00000,B01111,B10011,B10101,B10001,B01110};		//carateres spéciaux pour faire un petit vélo sur l'ecran
byte Char2[] = {B11001,B10000,B11111,B10001,B10010,B11100,B00000,B00000};
byte Char3[] = {B11100,B00000,B10000,B01110,B11001,B10101,B10001,B01110};



void setup() {
 
bluetooth.begin(9600);
  
Serial.begin(9600);
gyro.init();
gyro.zeroCalibrate(200,10);//sample 200 times to calibrate and it will take 200*10ms
  
 

lcd.begin(16, 2);		//parametrage de l'écran (colonnes et lignes)
lcd.createChar(1, Char1);		//création des characteres
lcd.createChar(2, Char2);
lcd.createChar(3, Char3);
 
  

pinMode(12, OUTPUT); //parametrage des differents ports pour les moteurs
pinMode(9, OUTPUT); 

 
pinMode(13, OUTPUT); 
pinMode(8, OUTPUT);  
   

    
lcd.setCursor(0, 0);
lcd.write(1);		//message de mise en route 
lcd.write(2);
lcd.write(3);
lcd.setCursor(0, 1);
lcd.print("BIKE LEARNER");
delay(800);
  
scrollright();     //animation pour le texte

uint8_t i=0;

}




void loop() {
 

uint8_t buttons = lcd.readButtons();


while( l==0)    //initialisation du programme
{		
    
    lcd.setCursor(0, 0);
   
    lcd.print("Choisir niveau :");		//selection du niveau de départ
    lcd.setCursor(0, 1);
    lcd.print("Niveau ");		//affichage
    lcd.print(level);
    
    if(level==1)
    {
    lcd.print("(Facile)");
    }
    if(level==2)
    {
    lcd.print(" (Moyen)");
    }
    if(level==3)
    {
    lcd.print("(Expert)");
    }
    
    buttons = lcd.readButtons();
    
    
  
    if(buttons & BUTTON_UP && level<3)    //niveau superieur
    {		
      level=level+1;
    }
    
    
    if(buttons & BUTTON_DOWN && level>1)    //niveau inferieur
    {		
      level=level-1;
    }
   
    
    if (buttons & BUTTON_SELECT)    //selection du niveau
    {		
      lcd.clear();  
      l=1;		//pour quitter l'initialisation
      debut2=millis();		//initialisation des "debuts"
      debut=millis();
    }
} 

float ax,ay,az;
gyro.getAngularVelocity(&ax,&ay,&az);  
   
lcd.setCursor(0, 0);
lcd.print(cm);		// hauteur des roulettes
lcd.print("cm"); 
lcd.setCursor(4, 0);
lcd.print(az);// angle detecte
  
  if ( az > 50 && roulette==1 || az < -50 && roulette==1 )    //condition pour baisser les roulettes
  { 
  
  descente();		//sous programme pour baisser les roulettes
  
  }
  
  if (roulette==0)
  {  //condition pour relever les roulettes
 

  
     if(delta1<=10)
      { 	//annalyse sur 10s pour savoir si on peut relever les roulettes ou non
   
        delta1=delta1+0.2;		//calcul du temps d'analyse
        a=a+0.2;		//calcul temps passé les roulettes baissées
        lcd.setCursor(12, 1);
        lcd.print(delta1);		//affichage sur l'écran du temps d'analyse
    
    
          if (az > 50 || az < -50 )
          { 		//on regarde si l'enfant est stable/depasse un certain angle
       
              n=n+1;		 //on compte le temps passé par l'enfant 
              lcd.setCursor(7, 1);
              lcd.print(n);      //affichage sur l'écran de n    
              lcd.setCursor(12, 1);
              lcd.print(delta1);		//affichage sur l'écran du temps d'analyse
           }  
    
      }
  
  
    else 
    {
    
      if (n<=10)
      { 		// si l'enfant a été stable pendant les 10 sec on releve les roulettes
      
         levage();		//sous programme pour lever les roulettes   
      } 
    
       delta1=0;		//reinitialisation des valeurs
       lcd.clear();
       n=0; 
       p=p+10; 		//temps passé les roulettes baissées
       lcd.setCursor(9, 1);
       lcd.print(p);
    }

}
  

t=((millis()-debut2)/ 1000);    	//calcul du temps de la session
lcd.setCursor(0, 1);	//affichage du temps de la session
delta=((millis()-debut) / 1000);	//calcul du temps passé sur l'analyse
lcd.print(delta);
lcd.print("s");
  
if(delta>60)
{   
   
 if(p<15 && level<3 )
    {  	//si l'enfant a passé moins de 30s les roulettes baissées
     
       if (roulette==1)
       {
       descente();
       } 
     
       level=level+1;		//alors l'enfant passe au niveau du dessus
       step = map(level, 1, 3, 2000, 4000); //mappage des steps en fonction du niveau  
       lcd.clear();
       lcd.print("LEVEL UP");		//affichage sur l'ecran
       delay(1000);
       lcd.clear();
     
    }


if(level==3 && p==10)
        {
           rmv=true;
           lcd.clear();
       lcd.print("Vous êtes prêt");		//affichage sur l'ecran
       delay(1000);
       lcd.clear();
        }      
    
  		
    debut=millis();	//nouvelle valeur de depart des 60s d'analyse
    p=0;   
    lcd.clear();

 
 }  

lcd.setCursor(11, 0);		//affichage du niveau sur l'eran
lcd.print("lvl ");
lcd.print(level);
 
buttons=lcd.readButtons();
  
  if (buttons & BUTTON_SELECT)    
  {      

    end=1;
     lcd.clear();
  
  }  
  
  while(end==1)
  {
    lcd.setCursor(0, 0);		//confiration pour eviter mauvaise manip'
    lcd.print("Finir session ?");
    lcd.setCursor(0, 1);		
    lcd.print("CONFIRMER ");
    
    buttons = lcd.readButtons();
  
    		//confirmation fin de session par un secon appui

      if (buttons & BUTTON_SELECT)
      {
        end=0;
        quit=0;
        lcd.clear();
  


        bluetooth.print(t);
        bluetooth.print("s");
        bluetooth.print("|");
        bluetooth.print(a);
        bluetooth.print("s");
        bluetooth.print("|");
   float r=a/t*100;		//calcul du pourcentage du temps passé roulettes baissées
        bluetooth.print(r);
        bluetooth.print("%");
        bluetooth.print("|");
        bluetooth.print(level);
        bluetooth.print("|"); 
   
         if(rmv==true)
         {
          bluetooth.print("OUI");
         }
          
          else
          {
            bluetooth.print("NON");
          }
  
           while(quit==0)
           {		//on affiche un menu avec les stats de l'enfant
    
              buttons = lcd.readButtons();		//lecture des differents boutons qui controle le menu
    
     
               if(buttons & BUTTON_UP)    //si "up" alors page suivante
               { 		
                WhichScreen++;
                lcd.clear();
               }
              
               if(buttons & BUTTON_DOWN)    //si "down" alors page precedente
               { 		
               WhichScreen--;
               lcd.clear();
               }  
              
               if (WhichScreen > 6)    //verification que la page suivante existe
               {		
               WhichScreen = 1;
               }
              
               if (WhichScreen < 0)
               {
               WhichScreen = 6;
               }   
      
      
                switch(WhichScreen) {		//on change de page
                
                case 1:		//affichage de la page 1
                {
                 lcd.setCursor(0,0); // Column, line
                 lcd.print("STATISTIQUES:");//indication que se sont les stats
  
   
                }
                break;
   
                case 2:		//affichage de la page 2
                {
        
                lcd.setCursor(0,0); // Column, line
                lcd.print("Pourcentage:");
                lcd.setCursor(0,1);
                lcd.print(r);		//affichage
                lcd.print("%");
                }
                break;
   
                case 3:		//affichage de la page 3
                {
        
                lcd.setCursor(0,0); // Column, line
                lcd.print("Temps session:");
                lcd.setCursor(0,1);
                lcd.print(t);		//affichage du temps de la session
                lcd.print("s");
                }
                 break;		
   
                case 4:		//affichage de la page 4
                {
        
                lcd.setCursor(0,0); // Column, line
                lcd.print("Temps baissees:");
                lcd.setCursor(0,1);
                lcd.print(a);		//affichage du temps passé les roulettes baissées
                lcd.print("s");
                }
                break;
   
                case 5:		//affichage de la page 5
                {
                  
                lcd.setCursor(0,0); // Column, line
                lcd.print("Niveau atteint:");
                lcd.setCursor(0,1);
                lcd.print("Niveau ");
                lcd.print(level);		//affichage du niveau atteint
                }
                break;
   
                case 6:		//affichage de la page 6
                {
        
                lcd.setCursor(0,0); // Column, line
                lcd.print("QUITTER");		//page pour quitter le programme
                    if (buttons & BUTTON_SELECT)      //selection par appui sur "select"
                    {		
                      lcd.clear();
                      lcd.print("BYE BYE");		//message d'au revoir
                      lcd.setCursor(0, 1);
                      lcd.write(1);		//message de mise en route 
                      lcd.write(2);
                      lcd.write(3);
                      delay(1000);
                      scrollright();		//animation
                      lcd.clear();
                      a=0;		//reinitialisation
                      l=0;
                      delta1=0;
                      n=0;
                      p=0;
                      quit=1;
                      end=0;
                      delta=0;
                      level=1;
                      delay(2000);
                    }
        
                }
                break;
                
                case 0:
                {
                 
                }
                break;
                }
  
          }    
      
      
           if(roulette==1)    //on baisse les roulettes si elles sont en position haute
           {		
            descente();
           }
    
    }

     if(buttons & BUTTON_DOWN||buttons & BUTTON_UP)    //si fausse manip,on revient au programme au lieu de le quitter
     {		
     lcd.clear();
     end=0;
     }

  }

}


void levage(){		//sous programme pour lever les roulettes
  
digitalWrite(12, LOW);  //descente des roulettes
digitalWrite(9, LOW);   
   
analogWrite(3, 255); 
   
 	 
 	 
delay(step);  		//durée d'execution en fonction du niveau donc plus au moins haut      

digitalWrite(9, HIGH);		 //arret des moteurs
digitalWrite(8, HIGH);   
	 
 
   
 	 
cm=cm+2*level;   // calcul de l'hauteur en cm
roulette=1; 		
}

void descente(){		//sous programme pour decendre les roulettes
  
digitalWrite(12, HIGH);  //descente des roulettes
digitalWrite(9, LOW);   
   
analogWrite(3, 255); 
   
 	 
 	 
delay(step);  		//durée d'execution en fonction du niveau donc plus au moins haut      

digitalWrite(9, HIGH);		 //arret des moteurs
digitalWrite(8, HIGH);   
  
cm=cm-2*level;   // calcul de l'hauteur en cm
roulette=0; //etat des roulettes
  
} 
  
  


void scrollright(){		//sous programme pour l'animation qui balaye l'ecran vers la droite
  
  lcd.scrollDisplayRight();		//deplacement de l'écran avec accélération
    delay(500);
  lcd.scrollDisplayRight();
    delay(450);
  lcd.scrollDisplayRight();
    delay(400);
  lcd.scrollDisplayRight();
    delay(350);
  lcd.scrollDisplayRight();
    delay(300);
  lcd.scrollDisplayRight();
    delay(250);
  lcd.scrollDisplayRight();
    delay(200);
  lcd.scrollDisplayRight();
    delay(150);
  lcd.scrollDisplayRight();
    delay(100);
  lcd.scrollDisplayRight();
    delay(50);
  lcd.scrollDisplayRight();
    delay(50);
  lcd.scrollDisplayRight();
    delay(50);
  lcd.scrollDisplayRight();
    delay(50);
  lcd.scrollDisplayRight();
    delay(50);
  lcd.scrollDisplayRight();
    delay(50);
  lcd.scrollDisplayRight();
    delay(50);
  
  
  
  lcd.scrollDisplayLeft();		//remise en place de l'ecran
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.scrollDisplayLeft();
  lcd.clear();
  
}

