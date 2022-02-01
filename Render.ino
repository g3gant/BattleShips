void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  FastLED.show();
  EVERY_N_MILLISECONDS( 20 ) { gHue++; }
  delay(50);  
}

void Merge3Arrays(byte renderArr[10][10], byte  layer1arr[10][10], byte  layer2arr[10][10],byte  layer3arr[10][10])
{
  for (int i=0;i<=9;i++)
    for (int j=0;j<=9;j++){

              renderArr[i][j] = layer1arr[i][j]>0?1:0;
              
              renderArr[i][j] = layer2arr[i][j]>0?2:renderArr[i][j];
              
              renderArr[i][j] = layer3arr[i][j]>0?3:renderArr[i][j];


    }
}
void Merge2Arrays(byte renderArr[10][10], byte  layer1arr[10][10], byte  layer2arr[10][10])
{
  for (int i=0;i<=9;i++)
    for (int j=0;j<=9;j++){

              renderArr[i][j] = layer1arr[i][j]>0?2:0;
              
              if (layer2arr[i][j] == 1)
              {
                renderArr[i][j] = 1;
              }
              if (layer2arr[i][j] == 2)
              {
                renderArr[i][j] = 4;
              }
              if (layer2arr[i][j] == 3)
              {
                renderArr[i][j] = 3;
              }
              //renderArr[i][j] = layer2arr[i][j]>0?3:renderArr[i][j];
              



    }
}
void clear_render()
{

    for (int i=0;i<=9;i++)
      {
        for (int j = 0; j<=9; j++)
        {
              
                arr_1[i][j] = 0;
                arr_2[i][j] = 0;
                arr_3[i][j] = 0;
                arr_4[i][j] = 0;
    
        }
      }
};

void m2l(byte c[10][10], byte d[100]) // Converting 2D to 1D array
{
    for (int i = 0; i<=9; i++)
    {
      for (int j = 0; j<=9; j++)
      {
        d[(i*10)+j] = c[j][i];
      }
    }
}


void render(byte renderArr[10][10])
{
    
  //MergeArrays(renderArr,arr_1,arr_2,arr_3);
  m2l(renderArr,ships_3);
  int col = 0;
  for (int i=0; i<=99;i++)
  {
    col = ships_3[i];
    if (col == 1){             
      leds[i] = CRGB (0,0,50);
    }
    
    if (col == 2 ){
      leds[i] = CRGB (0,50,0);
    }

    if (col == 3 ){
      leds[i] = CRGB (50,0,0);
    }
    if (col == 4){
      leds[i] = CRGB (50,30,10);
    }
    
 //Serial.println();
 delay(0.3);
  }
  FastLED.show();
  for (int i=0; i<=99;i++)
    {
      leds[i] = CRGB ( 0, 0, 0);
    }
  
}
