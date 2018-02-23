/*
  By: Navraj Saini
  login id: sair4210
*/
#include "cachelab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void hel()
{
   printf("-v for verbose(optional) \n");
   printf("-s <s> number of set index bits (S = 2^s is the number of sets), \n");
   printf("-E <E> Associativity, -b <b> number of block bits B = 2^b is the block size, \n");
   printf("-t <tracefile> Number of the valgrind trace to reply");
}

//
typedef struct
{
   int s = 0; // the cache sets
   int b = 0; // block size
   int E = 0; // number of cache lines in a set
   int S = 0; // num of sets S = 2**s
   int B = 0; // B = 2 ** b

   int hits = 0;
   int miss = 0;
   int evict = 0;
   
} param;

//a struct for each set line
typedef struct
{
   int valid; // valid bit
   char *block;  //block
   long long int tag; // tag bit
   int last_line; // the last line used
} set_l;

//cache set
typedef struct
{
   set_l *lines;
} set_s;

//cache struct
typedef struct
{
   set_s *sets;
} cache;


// build the cache based on s, E, B
cache b_c(int set_n, int line_n, int blk_sz)
{
   // ini the cache, set and line
   cache new_c;
   set_s sets;
   set_l line;

   //set index and line index
   int line_ind;

   // allocate storage for the sets in cache
   new_c.sets = (set_c *) malloc(sizeof(set_c) * set_n);

   //i: set index, is 0 to s-1
   for (int i = 0; i < set_n; i++)
   {
      //alloc storage for the lines in cache
      sets.lines = (set_l *) malloc (sizeof(set_l) * line_n);

      new_c.sets[i] = sets;

      for(int j = 0; j < line_n; j++)
      {
	 line.last_line = 0;
	 line.valid = 0;
	 line.tag = 0;

	 sets.lines[j] = line;
      }
   }
   return new_c;
}// done building the cache

//simulate the cache
param sim_c (cache c, param par, long long int addr)
{
   int c_full = 1; // is the cache full?

   int lines_n = par.E;
   int prev = par.hits;

   long long int temp = addr << (64 - (par.s + par.b)); // bit shift left the tag size
   long long int set_ind = temp >> ((64 - (par.s+par.b)) + par.b); // set index is bit shifted right

   long long int in_tag = addr >> (par.s+par.b);

   set_s q_set = c.sets[set_ind];

   for (int i = 0; i < lines_n; i++)
   {
      set_l line = q_set.lines[i];

      if(line.vaid)
      {
	 if (line.tag == in_tag) //hit
	 {
	    line.last_line++;
	    par.hits++;
	    q_set.lines[i] = line;
	 }
      }
      else if(!(line.valid && c_full))
	 c_full = 0;
      
   }
   if (prev == par.hits)
      //miss
      par.miss++;
   else
      return par;
   
}

//LRU function cause it's better than jaming everything in sim cache func

int LRU (set_s c, param par, int *used_l)
{

   int max = c.lines[0].last_line;
   int min = c.lines[0].last.line;

   
   
}

int main(int argc, char* argv[])//number of args and the arguments
{
   printf("Argc %d: ", argc);
   printf("\n");
   //test printing the arguements
   for (int i = 1; i < argc; i++)
   {
      printf("Args %d : %s\n", i, argv[i]);
   }
   //works!!!
   
   int set_in = 0;//set_in: variable to hold the set index
   int assoc_in = 0;// assoc_in: variable to hold the associativity
   int blk_in = 0;// blk_in: variable to hold the number of blocks
   char buff[255];// buffer to store the info read in from the file
   char *buffer = NULL;
   buffer = buff;
   int max_in = 255;// max number of chars read in before fgets exits
   int miss = 0;// variable to hold the number of misses
   int hit = 0;// variable to hold the number of hits
   int evict = 0;// variable to hold the number of evicts

   int mem = 0;// variable to hold the memory address
   int sz = 0;// variable to hold the size, whatever that does lol
   
   //file type variable to open and read the file
   FILE *f_read;
   
   //if there is no -h/-v/-hv
   if (argc == 9)
   {
      printf("argc: %d", argc);
      
      if(strcmp(argv[1], "-s") == 0)
      {
	 printf("\nin -s \n");
	 set_in = atoi (argv[2]);// get the string index from the input
	 
	 //test
	 printf ("%d", set_in);
	 printf("\n");
      }

      if(strcmp(argv[3], "-E") == 0)
      {
	 printf ("\nin -E\n");
	 assoc_in = atoi (argv[4]);

	 //test
	 printf("%d", assoc_in);
	 printf("\n");
      }

      if(strcmp(argv[5], "-b") == 0)
      {
	 printf ("\nin -b\n");
	 blk_in = atoi (argv[6]);

	 //test
	 printf("%d", blk_in);
	 printf("\n");
      }

      //get the trace file and run the simulator
      if (strcmp(argv[7], "-t") == 0)
      {
	 f_read = fopen(argv[8], "r"); // open the trace file and set it so we can read it

	 if (!f_read)// if we can't read the file
	 {
	    printf("file isn't there yo\n");
	 }
	 
	 else
	 {
	    // while it's not the end of file keep reading in the input
	    // set up the simulator and run it
	    while (!feof(f_read))
	    {
	       // buff: to store the strigs read in from f_read
	       // max_in: max number of characters to read in
	       // f_read: the file to read in from
	       fgets(buff, max_in, f_read);
	       // this isn't working properly.....
	       // needs to check if the 0th location in the array is a space
	       
	       if (buff[0] == ' ')
	       {
		  if (buff[1] == 'L' && mem == 0) // if the first call is a load
		  {
		     mem++;
		     sz++;
		     mem = atoi (&buffer[3]);
		     sz = atoi (&buffer[5]);

		     hit++; hit = 0;
		     miss++;
		     miss = 0;
		     evict++; evict = 0;
		     printf("\n\n if in while loop");
		     printf("\nmem: %d", mem);
		     printf("\nsize: %d", sz);
		     printf("\n");
		  }// end of if in the if statement

		  else if(buff[1] == 'S' && mem == 0)//if the first call is a store
		  {
		     
		  }
		  else
		     printf("why you no read the L || M || S ||?\n");
	       }//end of if in the while loop
	       else
		  printf("why you no read space?\n");                        //ERROR
	    }// end of while
	    
	 }//end of else
	 
      }// end of if statement for -t
      
   }//end of if statement for if (argc == 9)
   
   //if there is a -h/-v/-hv
   else if (argc == 10)
   {
      if (strcmp(argv[1], "-h") == 0)
      {
	 printf ("in -h \n");
	 hel();
      }

      if(strcmp(argv[1], "-v") == 0)
      {
	 //shows all the misses/hits/evicts when the program is run... need to implement
      }

      if (strcmp(argv[1], "-hv") == 0)
      {
	 printf("in -hv \n");
	 hel();
	 //shows all the misses/hits/evicts when the program is run... need to implement
      }

      if(strcmp(argv[2], "-s") == 0)
      {
	 printf("\nin -s \n");
	 set_in = atoi (argv[3]);// get the string index from the input
	 
	 //test
	 printf ("%d", set_in);
	 printf("\n");
      }

      if(strcmp(argv[4], "-E") == 0)
      {
	 printf ("\nin -E\n");
	 assoc_in = atoi (argv[5]);

	 //test
	 printf("%d", assoc_in);
	 printf("\n");
      }

      if(strcmp(argv[6], "-b") == 0)
      {
	 printf ("\nin -b\n");
	 blk_in = atoi (argv[7]);

	 //test
	 printf("%d", blk_in);
	 printf("\n");
      }
      //get the trace file and run the simulator
      if (strcmp(argv[8], "-t") == 0)
      {
	 f_read = fopen(argv[9], "r"); // open the trace file and set it so we can read it

	 if (!f_read)// if we can't read the file
	 {
	    printf("file isn't there yo\n");
	 }

	 else
	 {
	    //while it's not the end of file keep reading in the input
	    while (!feof(f_read))
	    {
	       // buff: to store the strigs read in from f_read
	       // max_in: max number of characters to read in
	       // f_read: the file to read in from
	       fgets(buff, max_in, f_read);

	       // if -v is there, print all the info in the file as well
	       if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "-hv") == 0)
	       {
		  
	       }
	       
	    }// end of while in else
	    
	 }//end of else
	 
      }// end of if statement for -t
      
   }// end of else if (argc == 10)
   printSummary(0, 0, 0);
   return 0;
}
