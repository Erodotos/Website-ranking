/** @file pagerank.c 
 *  @brief Implemet a simple version of the PageRank Algorithm (by Google). 
 * 
 * This program takes as input 2 files which contain
 * the id, name, and data of some netwok hosts. After reading these files,
 * it calculates using the PageRank Algorithm, the 
 * rang of among the hosts as well as the norm of the ranking.
 * 
 *  @author Erodotos Demetriou 
 *  @bug No known bugs. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Function prototypes.
int createArraysfromFiles(int **, char **, int *, char [], char[]); 

/** @brief This is the main function of the program
 *         
 *  This is the first function which is triggerd when the 
 *  program lunches. First of all main() check if the given
 *  arguments to the command line are correct. Then 3 array pointers 
 *  are created using malloc. These arrays will be filled with data latter
 *  in other function.  Finally main calls other functions which calculate 
 *  the desired result. 
 * 
 *  @param argc The number of given arguments
 *  @param *argv[] An array which contains the arguments given
 *  @return int. 
 */

int main(int argc, char const *argv[]){

   // Check commandline arguments
   if (argc != 3) {
      printf("Error in given arguments!\n");
      return EXIT_FAILURE;
   }

   // The following arrays are created to store the names of the
   // files we want to imports as data.
   char hostNames[100] = {};
   char hostGraph[100] = {};
   strcpy(hostNames,argv[1]);
   strcpy(hostGraph,argv[2]);

   // Initialize 3 array pointers which will be used latter in the 
   // program.
   int hosts=1;
   int **statsArray = (int **)malloc(hosts* sizeof(int*));
   char **namesArray = (char **)malloc(hosts* sizeof(char*));
   int *idArray = (int*)malloc(hosts*sizeof(int));
   
   // Call function createArraysfromFiles which will calculate and print the 
   // hosts rankings.
   createArraysfromFiles(statsArray,namesArray, idArray, hostNames, hostGraph );

   return 0;
}

/** @brief This function read the data from files, make the proper calculations and print results.
 *         
 *  In this function we create 2 pointer in order to read from 2 different files as the requested.
 *  First the program read the hostsname file and the hostgraph file.
 *  Malloc and calloc is used to allocate the proper size of memory according to the file contents.
 *  After that Google PageRank algorithm is used to calculate and output the results.
 * 
 *  @param statsArray This is a double pointer array where data will be stored in 
 *  @param namesArray This is a pointer array which stores the names of the hosts
 *  @param idArray This is a pointer array which stores the ids of the hosts
 *  @param hostNames The name of the file which contains hosts
 *  @param hostGraph The name of the file which contains info about the hosts connection
 *  @return int. 
 */

int createArraysfromFiles(int **statsArray, char **namesArray, int *idArray, char hostNames[], char hostGraph[]){

   // Initialise 2 file pointers in order to read the required data 
   //from files.
   FILE *fp1;
   FILE *fp2;
   fp1 = fopen(hostNames,"r");
   fp2 = fopen(hostGraph,"r");
   if (fp1==NULL || fp2 ==NULL){
      printf("This file cannot be opened. Error occured");
      EXIT_FAILURE;
   }

   int i = 0;        //This counter after reading the files will store the number of hosts in total.
   int length = 1;
   char buffer[100];

   //First i will read hostnames and ids.
   while(fgets(buffer,sizeof(buffer),fp1)){
      // Read the first token which is the host's id
      char *token = NULL;
      token = strtok(buffer, " ");
      idArray[i] = atoi(token);

      //Read the second token which is the host's name
      token = strtok(NULL, "\n");
      int nameSize = strlen(token);
      namesArray[i] = (char *)malloc(nameSize*sizeof(char));
      strcpy(namesArray[i], token);
      
      //Make arrays larger using realloc in order to fit the input data
      if (i==length){
         length = length * 2;
         int *idArray2 = (int *)realloc(idArray,length * sizeof(int)) ;
         if (idArray2 != NULL){
            idArray = idArray2;
         }
         char **namesArray2 = (char **)realloc(namesArray,length * sizeof(char *)) ;
         if (namesArray2 != NULL){
            namesArray = namesArray2;
         }
      }
      // Increase counter i which stores the number of hosts.
      i++;
   }

   //Create statsArray according to number of hosts
   int **statsArray2 = (int **)realloc(statsArray, i * sizeof(int *)) ;
   if (statsArray2 != NULL){
      statsArray = statsArray2;
   }   
   
   //Now i will read the hostgram
   char *token = NULL;
   while(fgets(buffer,sizeof(buffer),fp2)){
      // Create an array to store all the numbers in the same line of the setence
      int pointsCounter = 0;
      int points[100];
      // FInd next token and take into account multiple deimeters.
      token = strtok(buffer, " ->:");
      while (token != NULL) {
         points[pointsCounter++] = atoi(token);
         token = strtok (NULL, " ->:");
      }
      //Insert the data we read above into the stats array.
      int row = points[0];
      statsArray[row] = (int *)calloc(i,sizeof(int));
      // Check for cases there are not data to input in the array
      // and if the file format is correct. 
      if (pointsCounter == 2){
         continue;
      }else if (pointsCounter%2 == 0){
         printf("File format error!\n");
      }else{
         int j=0;
         for (j=1;j<pointsCounter;j+=2){
            int col = points[j];
            statsArray[row][col] = points[j+1];
         }
      }
   }
   //Close file pointers
   fclose(fp1);
   fclose(fp2);

   //Printing staff goes on
   printf("+++ Start of printing structure Oij +++\n");
   printf("The structure of each line is:\n");
   printf("src -> dest1:nlinks1 dest2:nlinks2 ... destk:nlinksk\n\n");

   int k,l;
   for (k=0;k<i;k++){
      printf("%d ->",idArray[k]);
      for (l=0;l<i;l++){
         if (statsArray[k][l] != 0){
            printf(" %d:%d", l, statsArray[k][l]);
         }
      }
      printf("\n");
   }
   printf("\n--- End of printing structure Oij ---\n\n\n");


   printf("+++ Start of printing structure Oj +++\n");
   printf("The structure of each line is:\n");
   printf("src ==> links_total\n\n");
   
   // Create an other array using malloc to store valuable info which we will use later
   int *Oj = (int *)calloc(i,sizeof(int));

   for (k=0;k<i;k++){
      printf("%d ==>",idArray[k]);
      int totalLinks = 0;
      for (l=0;l<i;l++){
         totalLinks += statsArray[k][l];   
      }
      Oj[k] = totalLinks;
      printf(" %d\n", Oj[k]);
   }
   printf("\n---End of printing structure Oj ---\n\n\n");


   printf("+++ Start of printing structure Iij +++\n");
   printf("The structure of each line is:\n");
   printf("dest <== hosts_total hostid1 hostid2 ... hostidk\n\n");

   int total=0;

   for (k=0;k<i;k++){
      printf("%d <==",idArray[k]);
      total=0;
      for (l=0;l<i;l++){
         if (statsArray[l][k] != 0){
            total++;
         }   
      }
      printf(" %d\t", total);
      for (l=0;l<i;l++){
         int total=0;
         if (statsArray[l][k] != 0){
            printf(" %d", idArray[l]);
         }   
      }
      printf("\n");
   }

   printf("\n--- End of printing structure Iij ---\n\n\n"); 

   printf("+++ Start of printing PR +++\n\n");

   // These arrays will store the data we will need to calculate the rank and Norm.
   float PRi[i];
   float PRi1[i];
   int position;
   for (position=0;position<i;position++){
      PRi[position] = 1;
   }
   float distance = 1.0;

   while(!(distance<=(0.02))){
      int m,n;
      float total;
      for (m = 0;m<i;m++){
         total = 0.0;
         for(n=0;n<i;n++){
            if (Oj[n]!=0){
               float sum = PRi[n]*statsArray[n][m]/Oj[n];
               total += sum;
            }
         }
         PRi1[m] = (1-0.85)/i + 0.85 * total ;
      }
      //Distance calculation
      float distanceTotal = 0.0;
      for (k=0;k<i;k++){
         distanceTotal+= (PRi[k]-PRi1[k]) * (PRi[k]-PRi1[k]);
         PRi[k] = PRi1[k];
      }
      distance = sqrt(distanceTotal);
   }

   //Norm Calculation
   float sum = 0.0;
   for (k=0;k<i;k++){
      sum+= PRi[k];
   }

   // Printing output
   printf("host_id\t\thost_name\thosts_rank\tnorm_host_rank\n");
   for (k=0;k<i;k++){
      printf("%d\t\t%s\t\t%.3f\t\t%.2f\n",idArray[k],namesArray[k],PRi[k],PRi[k]/sum);
   }

   printf("\n\n--- End of printing PR --- \n");

   //Free memory which was allocated dinamically using malloc.
   free(namesArray);
   free(idArray);
   free(Oj);
   for (k=0;k<i;k++){
      free(statsArray[k]);
   }
   free(statsArray);

   return 0;
}