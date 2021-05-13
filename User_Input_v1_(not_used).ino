#define NUM_LEDS 500

String inString = ""; 
int inChar = 0;
int16_t t = 0;
int pixelmap[NUM_LEDS+21] = {0};
long Array[NUM_LEDS] = {0};


void User_Input2()
{
    inString = Serial.readString();
    Serial.println();
    Serial.print("String: ");
    Serial.println(inString);
    Serial.println(); 
}



void FillArray()
{
  Serial.println("Filling Array... ");

  for (int i = 0; i < 521; i++)
  {
    pixelmap[i] = inString.charAt(i);  
  }
  
}


void PrintPixelmap()
{
  Serial.println();
  Serial.print("Printing pixelmap... ");
  Serial.println();
  
    for (int i=0; i < 500; i++)
     {
       Serial.print("pixelmap position ");
       Serial.print(i);
       Serial.print(" , value:  ");
       Serial.println(pixelmap[i]);
       //Serial.print("  ");
       //delay(200);
     }
}



void PrintArray()
{
  Serial.println();
  Serial.print("Printing array... ");
  Serial.println();
  
    for (int i=0; i < NUM_LEDS; i++)
     {
       Serial.print("Array position ");
       Serial.print(i);
       Serial.print(" , value:  ");
       Serial.println(Array[i]);
       //Serial.print("  ");
       //delay(200);
     }
}





void Col_conversion()
{
  for (int i = 0, n = 0; i < 521; i++, n++)
  {
    switch(pixelmap[i]) 
    {
    case 32:
        n--;
        break;
    case 48:
        Array[n] = 0xF0; //0x000000; -black
        break;
    case 49:
        Array[n] = 0xF1; //0xFFFFFF; -white
        break;
    case 50:
        Array[n] = 0xF2; //0xFF0000; -red
        break;
    case 51:
        Array[n] = 0xF3; //0xFFFF00; -yellow
        break;
    case 52:
        Array[n] = 0xF4; //0x00FF00; -green
        break;
    case 53:
        Array[n] = 0xF5; //0x0000FF; -blue
        break;
    default:
        Serial.print("Panic"); 
        
    }
  }
}



void setup() 
{
  Serial.begin(9600);
  while (!Serial) {;}
  Serial.println();
  Serial.print("Ready to receive pixeldata...");
  Serial.println();
}

void loop() 
{
   while (Serial.available() > 0)
  {
   User_Input2();
   FillArray();
   //PrintPixelmap();
   Col_conversion();
   PrintArray();
  }  
}
