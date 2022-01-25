class SHIP
{
  public:
    byte pos_x;
    byte pos_y;
    byte len;
    bool orient; // 0 - horizontal, 1 - vertical
    bool set = true;
    bool collision = false;
    byte coord[5][2];
    byte hitCntr = 0; // Hit counter
    void create(int x, int y, int lenght, bool orientation)
    {
      pos_x = x;
      pos_y = y;
      len = lenght;
      orient = orientation;
      coord[0][0] = x;
      coord[0][1] = y;
      
      if (orient)  // VERTICAL (orient = 1)
      {
         for (int i = 1; i<len;i++)
          {
            coord[i][0]=x;
            coord[i][1]=coord[i-1][1]+1;
          }   
      }
      
      
      if (!orient) // HORIZONTAL (orient = 0)
      {
         for (int i = 1; i<len;i++)
          {
            coord[i][0]=coord[i-1][0]+1;
            coord[i][1]=y;
          }   
      }
      
    }
    
    // Change the orientation
    bool rotate() // ROTATE THE SHIP (EXCHANGE THE COORDINATES)
    {
      int max_x;
      int max_y;
      int lng;
      bool result = false;
      
      max_x = coord[0][0]+len-1;
      max_y = coord[0][1]+len-1;

    
       if (!orient)
        {
         for (int i = 0; i<len; i++)
          {
            
            coord[i][0] = coord[i][0]-i;
            coord[i][1] = coord[i][1]+i;

            //if it goes out of field, move it back
             if (max_y >9)
             {
                 coord[i][1] = coord[i][1] - (max_y - 9);
             };
            
          };
         orient = true;
         result = true;
         
        } else
          {
     
         for (int i = 0; i<len; i++)
          {

            coord[i][0] = coord[i][0]+i;
            coord[i][1] = coord[i][1]-i;

            //if it goes out of field, move it back
             if (max_x >9)
             {
                 coord[i][0] = coord[i][0] - (max_x - 9);
             };
             
          }
         orient = false;
         result = true;
        }
      return result;
    };

    bool moveto(byte x, byte y)
    {
      byte new_x;
      byte new_y;
      int diff_x;
      int diff_y;
      byte max_x;
      byte max_y;

      bool result = false;
      max_x = 0;
      max_y = 0;
      diff_x = 0;
      diff_y = 0;
      new_x = x;
      new_y = y;
      if (orient) {
        max_x = new_x;
        max_y = new_y+len-1;

      } else {
        max_x = new_x+len-1;
        max_y = new_y;

      };
      
    
      
      if ((max_x <= 9) && (max_y <=9))
      {
              
          diff_x = new_x - coord[0][0];
          diff_y = new_y - coord[0][1];
    
          for (int i=0;i<len;i++)
          {
            coord[i][0] = coord[i][0]+ diff_x;
            coord[i][1] = coord[i][1]+ diff_y;
          };
          result = true;

      
      };

      return result;
    };
    
byte CheckHit(byte x, byte y) // Check given coordinates if they are hit and/or kill; 0 - miss, 1 - hit, 2 - kill
{
  bool hit=false;
  byte result=0;
  
  for (int i=0;i<len;i++)
      {
        if ((coord[i][0] == x) && (coord[i][1] == y)) {
          hit = true;
          break;
        }

      };
  if (hit == true) { hitCntr +=1;};
  if (hitCntr == len) {result = 2;} else {result = hit;};
  return result;
          
}
    
    
};
