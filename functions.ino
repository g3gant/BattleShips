void MoveShip()
{
  bool moved;
  byte new_x;
  byte new_y;
  bool x_changed = false;
  bool y_changed = false;
  
  moved = false;
  
  if (rotaryEncoderX.encoderChanged() & !chg_sel)
  {
    x_changed = true;  
    new_x = rotaryEncoderX.readEncoder();    
    Serial.print("X from encoder: ");
    Serial.print(new_x);
    Serial.println();
    
    moved = ships[sel_ship].moveto(new_x,y);
    
    draw_ships();
    //isOverlap(&ships[sel_ship]);
    
    
    if (!moved)
    {
      rotaryEncoderX.setEncoderValue(x);
    } else {
      x = new_x;
    };
  };


  if (rotaryEncoderY.encoderChanged() & !chg_sel)
  {
    y_changed = true;
    new_y = rotaryEncoderY.readEncoder();
    Serial.print("Y from encoder: ");
    Serial.print(new_y);
    Serial.println();
    moved = ships[sel_ship].moveto(x,new_y);
    draw_ships();
    // isOverlap(&ships[sel_ship]);
    if (!moved)
    {
      rotaryEncoderY.setEncoderValue(y);
    } else {
      y = new_y;
    };
  };


 
 if (chg_sel) 
 {
  chg_sel = false;
 }
}


void RotateShip()
  {
    bool rotated=false;
    rotated = ships[sel_ship].rotate();
    if (rotated)
    {
      set_encoder(ships[sel_ship].coord[0][0],ships[sel_ship].coord[0][1]);
    };
    draw_ships();
  }

void SelectNextShip()
{
      ships[sel_ship].set = true;
    
    sel_ship +=1;
    

    if (sel_ship > 4)
    {
      sel_ship = 0;
    }
    ships[sel_ship].set = false;

    chg_sel = true;
    
    set_encoder(ships[sel_ship].coord[0][0],ships[sel_ship].coord[0][1]);
    Serial.print("NEXT SHIP COORDINATES X,Y :");
    Serial.print(ships[sel_ship].coord[0][0]);
    Serial.print(",");
    Serial.print(ships[sel_ship].coord[0][1]);
    Serial.println();
    //draw_ships();
}

//=========================CHECK SHIPS OVERLAPPING=============
bool isOverlap(SHIP* chk_ship)
{

  int x_l, x_r;
  int y_u, y_d;
  int x,y;
  bool ovrlp=false;
  byte cnt = 0;
  bool ign_x_l = false;
  bool ign_x_r = false;
  bool ign_y_u = false;
  bool ign_y_d = false;
  bool result = false;




  
  
for (int i=0;i<chk_ship->len;i++){ // CHECKING OUTBOUNDS 
        x = chk_ship->coord[i][0];
        y = chk_ship->coord[i][1];
        
        x_l = ((x-1)<0?0:(x-1));
        x_r = ((x+1)>9?9:(x+1));
        y_u = ((y-1)<0?0:(y-1));
        y_d = ((y+1)>9?9:(y+1));


      ign_x_l = (x_l==x)?true:false; //IGNORE X-1

      ign_x_r = (x_r==x)?true:false; // IGNORE x+1

      ign_y_u = (y_u==y)? true:false; // IGNORE Y-1
    
      ign_y_d = (y_d==y)? true:false; //IGNORE Y+1
    
    if (!chk_ship->orient){ // IF SHIP IS HORIZONTAL, IGNORE X-1 AND X+1, Y

       // CHECKING ONLY ABOVE AND UNDER X
       if (!ign_y_u){
          cnt += arr_1[x][y_u] > 0;

       }
       if (!ign_y_d){
          cnt += arr_1[x][y_d] > 0;
       }
    }


    if (chk_ship->orient){ // IF SHIP IS VERTICAL, IGNORE X, Y-1 AND Y+1

       // CHECKING ONLY ABOVE AND UNDER X
       if (!ign_x_l){
          cnt += arr_1[x_l][y] > 0;

          
       }
       if (!ign_x_r){
          cnt += arr_1[x_r][y] > 0;

       }
    }
   

  cnt += arr_1[x][y] >= 2; // IF SHIPS COMPLETELY OVERLAPS
  
  }



//CHECKING EDGES
int len = chk_ship->len;
int f_x = chk_ship->coord[0][0];
int l_x = chk_ship->coord[len-1][0];
int f_y = chk_ship->coord[0][1];
int l_y = chk_ship->coord[len-1][1];

        x_l = ((f_x-1)<0?0:(f_x-1));
        x_r = ((l_x+1)>9?9:(l_x+1));
        y_u = ((f_y-1)<0?0:(f_y-1));
        y_d = ((l_y+1)>9?9:(l_y+1));

      ign_x_l = (x_l==f_x)?true:false; //IGNORE X-1

      ign_x_r = (x_r==l_x)?true:false; // IGNORE x+1

      ign_y_u = (y_u==f_y)? true:false; // IGNORE Y-1
    
      ign_y_d = (y_d==l_y)? true:false; //IGNORE Y+1

if (!chk_ship->orient){ // HORIZONTAL
  if (!ign_x_l){
    cnt += arr_1[x_l][f_y] > 0;
 
    if (!ign_y_u) cnt += arr_1[x_l][y_u] > 0;
    if (!ign_y_d) cnt += arr_1[x_l][y_d] > 0;
  }
  if (!ign_x_r){
    cnt += arr_1[x_r][l_y] > 0;
    if (!ign_y_u) cnt += arr_1[x_r][y_u] > 0;
    if (!ign_y_d) cnt += arr_1[x_r][y_d] > 0;
  }
  
}


if (chk_ship->orient){ //VERTICAL
  if (!ign_y_u){
    cnt += arr_1[f_x][y_u] > 0;
    if (!ign_x_l) cnt += arr_1[x_l][y_u] > 0;
    if (!ign_y_d) cnt += arr_1[x_r][y_u] > 0;
  }
  if (!ign_y_d){
    cnt += arr_1[l_x][y_d] > 0;
    if (!ign_x_l) cnt += arr_1[x_l][y_d] > 0;
    if (!ign_y_d) cnt += arr_1[x_r][y_d] > 0;
  }  
}


  chk_ship->collision = cnt>0;
  ovrlp = chk_ship->collision;
  return ovrlp;
}


//=============================================================




// ================ DRAW SHIPS ===========================

void draw_ships()
{
  clear_render();
  byte sh_x=0;
  byte sh_y=0;

  for (int s=0;s<5;s++)
  {
      
      for (int i=0;i<ships[s].len;i++)
      {
        sh_x = ships[s].coord[i][0];
        sh_y = ships[s].coord[i][1];

        arr_1[sh_x][sh_y] = arr_1[sh_x][sh_y] + 1;
       
      }
  }
  
  for (int s=0;s<5;s++)
  {
      if (!ships[s].set){
        for (int i=0;i<ships[s].len;i++)
          {
            sh_x = ships[s].coord[i][0];
            sh_y = ships[s].coord[i][1];
    
            arr_2[sh_x][sh_y] = 1;
           
          }

        
      }
  }  
  
  for (int s=0;s<5;s++)
  {
      if (ships[s].collision){
        for (int i=0;i<ships[s].len;i++)
          {
            sh_x = ships[s].coord[i][0];
            sh_y = ships[s].coord[i][1];
    
            arr_3[sh_x][sh_y] = 1;
           
          }

        
      }
  } 

  for (int i=0;i<5;i++) isOverlap(&ships[i]);
  
  Merge3Arrays(arr_4,arr_1,arr_2,arr_3);
  render(arr_4);
};
// ======================================================


void MoveAim()
{

  byte new_x;
  byte new_y;

  if (rotaryEncoderX.encoderChanged())
  {
     
    new_x = rotaryEncoderX.readEncoder();    

    x = new_x;
 
  };
  
  if (rotaryEncoderY.encoderChanged())
  {
     
    new_y = rotaryEncoderY.readEncoder();    

    y = new_y;
 
  };
  draw_aim (x,y);
}


void draw_aim(byte x, byte y)
{
  
for (int i=0;i<=9;i++)
      {
        for (int j = 0; j<=9; j++)
        {
              
                arr_1[i][j] = 0;

        }
      }
      
  arr_1[x][y] = 2;
  Merge2Arrays(arr_4,arr_1,arr_2);
  render(arr_4);
}


bool isOpponentReady()
{
  bool result=false;
  int packetSize = UDP.parsePacket();
  if (packetSize) {
        int len = UDP.read(packet, 255);
        if (len > 0)
        {
          packet[len] = '\0';
        }
        Serial.print("Packet received: ");
        Serial.println(packet);
        Serial.println("From: ");
        Serial.print(UDP.remoteIP());
        Serial.print(":");
        Serial.print(UDP.remotePort());
        String myString = String(packet);
        if (myString == "ReadyToShoot")
        {
          result = true;
        } 
  }
  return result;
}

void SendIamReadyToShoot()
{
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write("ReadyToShoot");
    UDP.endPacket();
}

bool SwitchGame()
{
  bool result = false;
  switch (game_mode)
  {
    case 2: {
    bool shipsCollisions = false;
    for (int i=0;i<5;i++)
    {
      shipsCollisions = shipsCollisions || ships[i].collision;
    }
    if (!shipsCollisions) {

      if (OpponentReadyToShoot){
        game_mode = 4; //Waiting for opponent shoot
      } else {
        game_mode = 3; //Shooting mode
      }
      SendIamReadyToShoot();
      clear_render();
      result = true;
    };
    };
    break;

    case 3:
    break;
  }
  return result; 
};

void MakeShot(byte x, byte y) // Sending the shot to the opponent and waiting on answer, we going to store all our shots in the array, with Hits and Misses
{
    bool getAnswer = false;
    int ShotResult = 0; // 0 - Miss, 1 - Hit, 2 - Kill
    //char msg = "";
    //msg = "s:"&x & "," & y;
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write("s:");
    UDP.write(x);
    UDP.write(",");
    UDP.write(y);
    UDP.endPacket();
    
    // Waiting on respond, add a sound of flying projectile
    while (!getAnswer)
    {
      ShotResult = CheckShotResult();
      delay(5);
      if (ShotResult >= 0) 
      {
        getAnswer = true;  
      }
      
    }
    // Answer recieved, mark the shot on the field
    if (ShotResult == 0)
    {
      arr_2[x][y] = 1; // Miss, delay (sound) and switch to opponent shooting
    }
    if (ShotResult == 1)
    {
      arr_2[x][y] = 2; // Hit, delay (sound), continue shooting
    }
    if (ShotResult == 2)
    {
      arr_2[x][y] = 2; // Kill, delay (sound), mark all neigbor cells as killed(red), count KilledShips+1, continue shooting
      markAsKilled(x,y);
    }
    
    
}

int CheckShotResult()
{
  int result=-1;
  int packetSize = UDP.parsePacket();
  if (packetSize) {
        int len = UDP.read(packet, 255);
        if (len > 0)
        {
          packet[len] = '\0';
        }
        Serial.print("Packet received: ");
        Serial.println(packet);
        Serial.println("From: ");
        Serial.print(UDP.remoteIP());
        Serial.print(":");
        Serial.print(UDP.remotePort());
        String myString = String(packet);
        
        if (myString == "Miss")
        {
          result = 0;
        };
        if (myString == "Hit")
        {
          result = 1;
        };
        if (myString == "Kill")
        {
          result = 2;
          
        };
  
          
        
  }
  return result;
}

void markAsKilled(byte x, byte y) //Recurcive function for marking killed ship
{
  if (arr_2[x][y] !=2 ) return;
  
  if (arr_2[x][y] == 2) {
   
    arr_2[x][y]=3;

    if (CheckNeighbors(x-1,y)){
      markAsKilled(x-1, y);
    }
    if (CheckNeighbors(x+1,y)){
      markAsKilled(x+1, y);
    }    
    if (CheckNeighbors(x,y-1)){
      markAsKilled(x, y-1);
    }
    if (CheckNeighbors(x,y+1)){
      markAsKilled(x, y+1);
    }

    }
  

}

bool CheckNeighbors(byte x, byte y)
{
  if (x>=0 && x<=9 && y>=0 && y<=9) { 
    return true;
    } else {
      return false;
    }
  
}
