#include<stdio.h>
#include<iostream>
#include<fstream>
#include<limits.h>
#include<vector>
#include<string>
#include<time.h>

#define max 50

using namespace std;
int const N = 600;

//DECLARE GLOBAL VARIABLES
int cap[N][N];
int hop[N][N];
int hop_cpy[N][N];
int delay[N][N];
int path1[N*N][N];
int path2[N*N][N];
int n,e;
int req;
int row = 0;
int vcid = 1000;

string topology_file;
string conn_req_file;
string routing_table_file;
string forwarding_table_file;
string conn_table_file;
string paths_file;
int approach;
int global_flag;

//FUNCTION TO READ TOPOLOGY FILE AND CREATE DELAY, CAPACITY AND HOP MATRICES
void create_matrices()
{
   fstream file;
    // OPENING FILE 
    file.open(topology_file,fstream::in);
    file >> n;
    file >> e;
    float dump;
    int top[e][4];

   //CREATE A TOPOLOGY MATRIX FROM INPUT FILE
    for(int i=0; i<e; i++)
      {
          for(int j=0; j<4;j++)
              file >> top[i][j];
        file >> dump;
      }
    file.close();

   //INITIALIZE ARRAYS
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
           {
            delay[i][j] = 0;
            hop[i][j] = 0;
            cap[i][j] = 0;
           }
    }

   //CREATING HOP,DELAY,CAPACITY ARRAYS
      int s,d;
      for(int i=0; i<e; i++)
      {
              s= top[i][0];
              d= top[i][1];
               if(global_flag == 0)
               {
                  hop[s][d] = 1;
                  hop[d][s] = 1;
               }
               else if(global_flag == 1)
               {
                  hop[s][d] = top[i][2];
                  hop[d][s] = top[i][2];
               }
              hop_cpy[s][d] = 1;
              hop_cpy[d][s] = 1;
              delay[s][d] = top[i][2];
              delay[d][s] = top[i][2];
              cap[s][d] = top[i][3];
              cap[d][s] = top[i][3];
      }
}

// FUNCTION TO FIND FIRST SHORTEST PATH
void calculate_first_path() 
{
   //CALCULATING FIRST SHORTEST PATH FOR ALL THE VERTICES  
   for(int startnode = 0; startnode<n; startnode++)
   { 
      int cost[max][max],distance[max],pred[max];
      int visited[max],count,mindistance,nextnode,i,j;
      for(i=0;i<n;i++)
         for(j=0;j<n;j++)
      if(hop[i][j]==0)
         cost[i][j]=9999;
      else
         cost[i][j]=hop[i][j];
      for(i=0;i<n;i++) 
      {
         distance[i]=cost[startnode][i];
         pred[i]=startnode;
         visited[i]=0;
      }
      distance[startnode]=0;
      visited[startnode]=1;
      count=1;
      while(count<n-1) 
      {
         mindistance=INT_MAX;
         for(i=0;i<n;i++)
            if(distance[i]<mindistance&&!visited[i]) 
            {
               mindistance=distance[i];
               nextnode=i;
            }
         visited[nextnode]=1;
         for(i=0;i<n;i++)
            if(!visited[i])
         if(mindistance+cost[nextnode][i]<distance[i]) 
         {
            distance[i]=mindistance+cost[nextnode][i];
            pred[i]=nextnode;
         }
         count++;
      }

      //CREATE PATH1 ARRAY OF SOURCE, DESTINATION, PATH_COST, AND PATH FOR EACH SOURCE, DESTINATION PAIR.
      for(i=0;i<n;i++)
      {
         if(i!=startnode)
         {
            path1[row][0]=startnode;
            path1[row][1]=i;
            path1[row][2]=distance[i];
            j=i;
            vector<int> tempDist;
            tempDist.push_back(i);
            do 
            {
                  j=pred[j];
                  tempDist.push_back(j);
            }while(j!=startnode);
                  int col = 4;
            for(auto i =tempDist.rbegin(); i!=tempDist.rend(); i++)
            {
                  path1[row][col] = *i;
                  col++;
            }
            path1[row][col]= 999;
         }
         row++;
      }
   }
}

//FUNCTION TO CALCULATE DELAY
void calculate_delay()
{
   //CALCULATE DELAY FOR PATH 1
   for(int i=0;i<n;i++)
   {
      int a,b;
      for(int j=0; j<n;j++)
      {
         int delayy = 0;
      int roww= n*i+j;
      
      for(int j=4;j<n;j++)
      {  
         if(path1[roww][j+1]==999)
         break;
         a=path1[roww][j];
         b=path1[roww][j+1];
         delayy=delayy + delay[a][b];       
      }
       path1[roww][3] = delayy;
      }
      
   }

   //CALCULATE DELAY FOR PATH 2
   for(int i=0;i<n;i++)
   {
      int a,b;
      for(int j=0; j<n;j++)
      {
         int delayy = 0;
      int roww= n*i+j;
      
      for(int j=4;j<n;j++)
      {  
         if(path2[roww][j+1]==999)
         break;
         a=path2[roww][j];
         b=path2[roww][j+1];
         delayy=delayy + delay[a][b];
      }
       path2[roww][3] = delayy;
      }
      
   }
}

//RESET HOP_CPY ARRAY
void init_hop_cpy()
{
   for(int i=0;i<n;i++)
      for(int j=0;j<n;j++)
      hop_cpy[i][j] = hop[i][j];
}

//FUNCTION TO FIND SECOND SHORTEST PATH BETWEEN TWO NODES
void second_path(int startnode,int dest)
{
   int cost[max][max],distance[max],pred[max];
   int visited[max],count,mindistance,nextnode,i,j;
   for(i=0;i<n;i++)
      for(j=0;j<n;j++)
   if(hop_cpy[i][j]==0)
      cost[i][j]=9999;
   else
      cost[i][j]=hop_cpy[i][j];
   for(i=0;i<n;i++) {
      distance[i]=cost[startnode][i];
      pred[i]=startnode;
      visited[i]=0;
   }
   distance[startnode]=0;
   visited[startnode]=1;
   count=1;
   while(count<n-1) {
      mindistance=INT_MAX;
      for(i=0;i<n;i++)
         if(distance[i]<mindistance&&!visited[i]) {
         mindistance=distance[i];
         nextnode=i;
      }
      visited[nextnode]=1;
      for(i=0;i<n;i++)
         if(!visited[i])
      if(mindistance+cost[nextnode][i]<distance[i]) {
         distance[i]=mindistance+cost[nextnode][i];
         pred[i]=nextnode;
      }
      count++;
   }
   //ONLY FIND SHORTEST PATH OF DESTINATION NODE
  i=dest;
    if(i!=startnode)
    {
        path2[row][0]=startnode;
        path2[row][1]=i;
        path2[row][2]=distance[i];
        j=i;
        vector<int> tempDist;
        tempDist.push_back(i);
        do 
        {
            j=pred[j];
            tempDist.push_back(j);
        }while(j!=startnode);
            int col = 4;
        for(auto i =tempDist.rbegin(); i!=tempDist.rend(); i++)
        {
            path2[row][col] = *i;
            col++;
        }
      
        path2[row][col]= 999;

    }
    row++;
}

//THIS FUNCTION CALLS SECOND_PATH() FOR EACH PAIR OF SOURCE AND DESTINATION
void calculate_second_path()
{
   for(int i=0;i<n;i++)
      {
      for(int j=0; j<n;j++)
      {
      int roww= n*i+j;
      
      for(int j=4;j<n;j++)
      {  
         if(path1[roww][j+1]==999)
         break;
         int a=path1[roww][j];
         int b=path1[roww][j+1];

      //SET EDGES OF SHORTEST PATH BETWEEN i AND j TO 0 --> i.e. REMOVE  THE EDGES FROM HOP MATRIX SO THAT WE'LL GET SECOND SHORTEST PATH
         hop_cpy[a][b] = 0;
         hop_cpy[b][a] = 0;
      }

      second_path(i,j);
      //RESET HOP_CPY ARRAY
      init_hop_cpy();
      }
   }
}
//FUNCTION TO FIND MINIMUM OF TWO NUMBERS
float findmin(float a,float b)
{
   if(a<b)
      return a;
   else
      return b;
      
}

//FUNCTION TO CHECK EACH REQUEST AND CREATE FORWARDING TABLE, CONNECTION TABLE AND PATHS FILE
void check_request()
{
   //READ CONNECTION REQUEST FILE
   fstream file,fwdfile,connfile,pathsfile;
   file.open(conn_req_file,fstream::in);
   //OPEN FORWARDING TABLE, CONNECTION TABLE AND PATHS FILE IN APPEND MODE
   fwdfile.open(forwarding_table_file,std::ofstream::out | std::ofstream::trunc);
   connfile.open(conn_table_file,std::ofstream::out | std::ofstream::trunc);
   pathsfile.open(paths_file,std::ofstream::out | std::ofstream::trunc);
   
   //READ NUMBER OF REQUESTS
   file >> req;
   pathsfile << "Number of requested connections are : "<<req <<"\n";
   fwdfile << "RouterID\tIncPortID\tVCID\t\tOutPortID\tVCID\n";
   connfile << "reqID     src      dest     path           vcid list      path cost\n";

   int src,dest;
   float bmin,bavg,bmax;
   int accepted_req = 0;

   //FOR EVERY REQUEST IN REQUEST FILE DO THIS
   for(int i=0; i<req; i++)
   {
      //READ SOURCE, DESTINATION, Bi_MIN, Bi_AVG, Bi_MAX 
      file>> src >> dest >>  bmin >> bavg >> bmax;

      float beq;

      //CALCULATE Bi_EQUIVALENT
      if(approach == 0)
      beq = findmin(bmax, bavg + 0.25 * (bmax-bmin));
      else if(approach == 1)
      {
         beq = bmax;
      }
      
      //TO FIND ROW NUMBER IN PATH TABLE
      int roww = n*src + dest;

      int a,b,flag=1,path=1;

      // CHECKING CAPACITY OF FIRST PATH
      for(int j=4;j<n;j++)
      {  
         if(path1[roww][j+1]==999)
         break;
         a=path1[roww][j];
         b=path1[roww][j+1];

         if (cap[a][b] < beq)
         {
            flag = 0; 
            path = 2;
            break;
         }      
      }

      //IF FIRST PATH CAN NOT SATISFY THE REQUEST THEN CHECK FOR SECOND PATH
      if(flag == 0)
      {
         flag=1;
         for(int j=4;j<n;j++)
         {  
            if(path2[roww][j+1]==999)
            break;
            a=path2[roww][j];
            b=path2[roww][j+1];
         
            if (cap[a][b] < beq)
            {
               flag = 0; 
               break;
            }     
         }
      }

      //IF THE REQUEST IS ACCEPTED, DO THIS
      if(flag == 1)
      {
       //cout<<"Conn "<< i <<" accepted."<<endl;
         vcid +=1000;
         accepted_req++;
        
         //IF ACCEPTED REQUEST IS FROM PATH 1
         if(path==1)
         {
               //SUBSTRACT BEQ FROM CAPACITY ARRAY FOR THE ACCEPTED REQUEST
            for(int j=4;j<n;j++)
            {  
               if(path1[roww][j+1]==999)
               break;
               a=path1[roww][j];
               b=path1[roww][j+1];
               cap[a][b] -= beq;
            }
            //PRINT ACCEPTED REQUEST IN FORWARDING TABLE FROM PATH 1
            for(int j=4;j<n;j++)
            {  
               fwdfile << path1[roww][j]<<"\t\t";
               if(j==4)
                  fwdfile << "---\t\t";
               else
               {
                  fwdfile <<path1[roww][j-1]<<"\t\t";
               }
                  
               fwdfile<<vcid<<"\t\t";
               if(path1[roww][j+1] == 999)
                  fwdfile <<"---\t\t";
               else
               { 
                  fwdfile  << path1[roww][j+1]<<"\t\t";
               }
               fwdfile <<vcid<<"\t\t"<<endl;
               if(path1[roww][j+1]==999)
               break;
            }
            //PRINT ACCEPTED REQUEST IN CONNECTION TABLE FROM PATH 1
            connfile<<i<<"\t"<<src<<"\t"<<dest<<"\t";
            for(int j=4;j<n;j++)
            {  
               if(path1[roww][j]==999)
               break;
              if(path1[roww][j+1]!=999)
               connfile << path1[roww][j] <<"->";
               else
               connfile << path1[roww][j];
            }
            connfile<<"\t\t\t";
            int pathCost = path1[roww][2];
            srand(time(0));
            int v = rand()%100;
            for(int i = 0; i<pathCost; i++)
               {
                  if(i==pathCost-1)
                  {
                     connfile<<vcid-(i*v) ;
                  }
                  else
                  connfile<<vcid-(i*v) <<" , ";
               }
            connfile<<"\t\t\t"<<pathCost<<"\n";

           
         }
         //IF ACCEPTED REQUEST IS FROM PATH 2
         if(path==2)
         {
            //SUBSTRACT BEQ FROM CAPACITY ARRAY FOR THE ACCEPTED REQUEST
            for(int j=4;j<n;j++)
            {  
               if(path2[roww][j+1]==999)
               break;
               a=path2[roww][j];
               b=path2[roww][j+1];
               cap[a][b] -= beq;  
            }
            //PRINT ACCEPTED REQUEST IN FORWARDING TABLE FROM PATH 2
            for(int j=4;j<n;j++)
            {  
               fwdfile << path2[roww][j]<<"\t\t";
               if(j==4)
                  fwdfile<< "---\t\t";
               else
               {
                  fwdfile<<path2[roww][j-1]<<"\t\t";
               }
                  
               fwdfile<<vcid<<"\t\t";
               if(path2[roww][j+1] == 999)
                  fwdfile<<"---\t\t";
               else
               { 
                  fwdfile << path2[roww][j+1]<<"\t\t";
               }
               fwdfile <<vcid<<"\t\t"<<endl;
               if(path2[roww][j+1]==999)
               break;
            }

            //PRINT ACCEPTED REQUEST IN CONNECTION TABLE FROM PATH 2
            connfile<<i<<"\t"<<src<<"\t"<<dest<<"\t";
            for(int j=4;j<n;j++)
            {  
               if(path2[roww][j]==999)
               break;
            
               if(path2[roww][j+1]!=999)
               connfile << path2[roww][j] <<"->";
               else
               connfile << path2[roww][j];
            }
            connfile<<"\t\t\t";
            int pathCost = path2[roww][2];
            srand(time(0));
            int v = rand()%100;
            for(int i = 0; i<pathCost; i++)
               {
                  if(i==pathCost-1)
                  {
                     connfile<<vcid-(i*v) ;
                  }
                  else
                  connfile<<vcid-(i*v) <<" , ";
               }
            connfile<<"\t\t\t"<<pathCost<<"\n";
         }
      }
   }
  pathsfile << "Number of accepted connections are : "<<accepted_req <<"\n";
}

//FUNCTION TO CREATE ROUTING TABLE
void create_routing_table()
{
   fstream rfile;
      rfile.open(routing_table_file,std::ofstream::out | std::ofstream::trunc);
      rfile <<"Src\tDest\tPath\t\t\t\tDelay\tPathCost"<<endl;


      //PRINTING PATH 1

      for(int i=0;i<n*n;i++)
      {

      //FIRST PRINT SOURCE DESTINATION
         for(int j=0;j<2;j++)
         { 
               if(path1[i][2] == 0)
               break;
               if(path1[i][j]==999)
               break;
            
            rfile<<path1[i][j]<<"\t"; 
         }
      //PRINT PATH 
         for(int j=4;j<n;j++)
         { 
               if(path1[i][2] == 0)
               break;
               if(path1[i][j]==999)
               break;
            if(path1[i][j+1]!=999)
                  rfile<<path1[i][j]<<" -> "; 
               else
               {
                  rfile<<path1[i][j]<<"\t\t\t\t";
               }
            
         }
      //PRINT DELAY, PATH COST
         for(int j=3;j>1;j--)
         { 
               if(path1[i][2] == 0)
               break;
               if(path1[i][j]==999)
               break;
            
            rfile<<path1[i][j]<<"\t"; 
         }
         if(path1[i][2] != 0)
         rfile<<endl;

      }


   //PRINTING PATH 2
   for(int i=0;i<n*n;i++)
   {
      //FIRST PRINT SOURCE DESTINATION
      for(int j=0;j<2;j++)
      { 
            if(path2[i][2] == 0)
            break;
            if(path2[i][j]==999)
            break;
         rfile<<path2[i][j]<<"\t"; 
      }

      //PRINT PATH
      for(int j=4;j<n;j++)
      { 
            if(path2[i][2] == 0)
            break;
            if(path2[i][j]==999)
            break;
            if(path2[i][j+1]!=999)
               rfile<<path2[i][j]<<" -> "; 
            else
            {
               rfile<<path2[i][j]<<"\t\t\t";
            }
         
      }
      //PRINT DELAY, PATH COST
      for(int j=3;j>1;j--)
      { 
            if(path2[i][2] == 0)
            break;
            if(path2[i][j]==999)
            break;
         
         rfile<<path2[i][j]<<"\t"; 
      }
      if(path2[i][2]!=0)
      rfile<<endl;
   }

}

int main(int argc,char *argv[])
{
  if(argc<8)
  {
     cout<<"\nNumber of parameters are less!!\nPlease give sufficient parameters\n\n";
     cout<<"Format of parameters is : \n./a.out topology_file conn_req_file routing_table_file forwarding_table_file conn_table_file paths_file 0/1(hop/delay) 0/1(optimistic/pessimistic)\n\n";
     return 0;
  }
  //INITIALIZE VARIABLES FROM COMMAND LINE INPUT
   topology_file = argv[1];
   conn_req_file = argv[2];
   routing_table_file = argv[3];
   forwarding_table_file = argv[4];
   conn_table_file = argv[5];
   paths_file = argv[6];
   global_flag = atoi(argv[7]);
   approach = atoi(argv[8]);
   
   //CALL FUNCTIONS
   create_matrices();
   calculate_first_path();

   //RESET ROW TO 0
   row=0;

   calculate_second_path();
   calculate_delay();
   create_routing_table();
   check_request();
      
   return 0;
}