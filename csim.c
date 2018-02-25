/*
  By: Navraj Saini
  login id: sair4210
*/
#include "cachelab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int verbose = 0;// global variable so it's easier to launch the -v flag

void hel()
{
   printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
   printf("Options: \n");
   printf("  -h         Print this help message.\n");
   printf("  -v         Optional verbose flag.\n");
   printf("  -s <num>   Number of set index bits.\n");
   printf("  -E <num>   Number of lines per set.\n");
   printf("  -b <num>   Number of block offset bits.\n");
   printf("  -t <file>  Trace file.\n\n");
   printf("Examples:\n");
   printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
   printf("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

//parameter struct to save the variables to use in functions
typedef struct
{
   int s; // the cache sets
   int b; // block size
   int E; // number of cache lines in a set
   int S; // num of sets S = 2**s
   int B; // B = 2 ** b

   int hits;// var to hold the number of hits
   int miss;// "    "  "    "    "     " misses
   int evict;//"    "  "    "   "      " evictions
   
} param;

//struct for each set line
typedef struct
{
   int valid; // valid bit
   char *block;  //block
   unsigned long long int tag; // tag bit
   int last_line; // the last line used
} set_l;

//cache set struct
typedef struct
{
   set_l *lines;
} set_c;

//cache struct
typedef struct
{
   set_c *sets;
} cache;


// build the cache based on s, E, B
cache b_c(int s, int E, int B)
{
   // ini the cache, set and line
   cache new_c;
   set_c sets;
   set_l line;


   // allocate storage for the sets in cache
   new_c.sets = (set_c *) malloc(sizeof(set_c) * s);

   
   for (int i = 0; i < s; i++)
   {
      //alloc storage for the lines in cache
      sets.lines = (set_l *) malloc (sizeof(set_l) * E);

      new_c.sets[i] = sets;// place a new set at every line

      // initialze the valid, tag and last line to 0
      // and ini all the lines to the storage for the lines
      for(int j = 0; j < E; j++)
      {
	 line.last_line = 0;
	 line.valid = 0;
	 line.tag = 0;

	 sets.lines[j] = line;
      }
   }
   return new_c;
}// done building the cache

//LRU function cause it's better than jaming everything in sim cache func
int LRU (set_c c, param par, int *used_l)
{

   //most and least recently used lines
   int most_used = c.lines[0].last_line; // max used
   int least_used = c.lines[0].last_line; // min used

   int min = 0; //the LRU that will be returned
   
   set_l line;

   // search through all the lines, if the current min is > the number
   // of times the line is accessed store the index and return it
   for (int i = 1; i < par.E; i++)
   {
      line = c.lines[i];
      
      if (most_used < line.last_line)
      {
	 most_used = line.last_line;
      }
      if (least_used > line.last_line)
      {
	 min = i;
	 least_used = line.last_line;
      }
   }

   used_l[0] = least_used;
   used_l[1] = most_used;

   return min;
}

//simulate the cache
param sim_c (cache c, param par, unsigned long long int addr, char op, int size)
{
   int c_full = 1; // is the cache full?

   int lines_n = par.E;
   int prev = par.hits;
   int sz = (64 - (par.s + par.b));
   unsigned long long temp = addr << sz; // bit shift left the sz size
   unsigned long long set_ind = temp >> (sz + par.b); // set index is bit shifted right

   unsigned long long in_tag = addr >> (par.s+par.b);
   
   set_c q_set = c.sets[set_ind];
   for (int i = 0; i < lines_n; i++)
   {
      set_l line = q_set.lines[i];
      
      if(line.valid)
      {
	 if (line.tag == in_tag) //hit
	 {
	    line.last_line++;
	    par.hits++;
	    q_set.lines[i] = line;
	    if (verbose == 1)
	       printf("%c %llx,%d hit\n", op, addr, size);
	 }
      }
      else if(!(line.valid && c_full))
	 c_full = 0;
   }
   if (prev == par.hits)
   {
      //miss
      par.miss++;
      if(verbose == 1)
	 printf("%c %llx,%d miss\n", op, addr, size);
   }
   else
      return par;// return cause the data is in the cache

   //else we continue with a miss and evict based on the LRU
   
   int *used_l = (int*) malloc(sizeof(int)*2);// alloc storage for the used lines to store the min and max
   int min_ind = LRU(q_set, par, used_l);// get the index for the LRU to evict

   if(!c_full)//if the cache is not full
   {
      set_l line1;
      int emp_ind = 0;// empty line index
      //find the empty line index
      for (int i = 0; i < par.E; i ++)
      {
	 line1 = q_set.lines[i];
	 if (line1.valid == 0)
	    emp_ind = i;
      }
      // set the valid, tag and last line accessed
      q_set.lines[emp_ind].valid = 1;
      q_set.lines[emp_ind].tag = in_tag;
      q_set.lines[emp_ind].last_line = used_l[1]+1;
   }
   else //evict
   {
      par.evict++;
      if (verbose == 1)
	 printf("%c %llx,%d evict\n", op, addr, size);
      q_set.lines[min_ind].tag = in_tag;
      q_set.lines[min_ind].last_line = used_l[1]+1;
   }
   free(used_l);
   return par;
}

int main(int argc, char* argv[])//number of args and the arguments
{
   cache myCache;
   param par;
   
   // initialize the hits misses and evicts
   par.hits = 0;
   par.miss = 0;
   par.evict = 0;
   
   char op; // takes the operation
   int size;// variable to store the size to pass to sim_c just in case verbose is set
   unsigned long long int addr;
   //file type variable to open and read the file
   FILE *f_read;
   //if there is no -h/-v/-hv
   if (argc == 9)
   {
      if(strcmp(argv[1], "-s") == 0)// if the first arguement is -s
      {
	 par.s = atoi (argv[2]);// get the string index from the input
	 par.S = pow(2.0, par.s);// and compute S (2 ** s)
      }

      if(strcmp(argv[3], "-E") == 0)// if the 3rd arguement is -E
      {
	 par.E = atoi (argv[4]);// get the associativity (num of lines per set)
	 
      }

      if(strcmp(argv[5], "-b") == 0)// if the 5th arguement is -b
      {
	 par.b = atoi (argv[6]);// get the number of block bits
	 par.B = pow(2.0, par.b);// compute the block size (2**b)
      }

      
      
      myCache = b_c(par.S, par.E, par.B);// make the cache so there isn't a seg fault later...
      //get the trace file and run the simulator
      if (strcmp(argv[7], "-t") == 0)
      {
	 f_read = fopen(argv[8], "r"); // open the trace file and set it so we can read it
	 //-----------------------------------------------------------------------------------------
	 // start of reading the file and starting the simulator
	 //-----------------------------------------------------------------------------------------
	 if (!f_read)// if we can't read the file
	 {
	    printf("file isn't there yo\n");
	 }
	 
	 else
	 {
	    
	    while (fscanf(f_read, " %c %llx,%d", &op, &addr, &size) == 3)// while there are 3 inputs that fit op, addr and size
	    {
	       if (op == 'I')// don't do anything if the operation is I
	       {
	       }
	       if (op == 'L')// run the sim once if the operation is L
	       {
		  par = sim_c(myCache, par, addr, op, size);
	       }
	       if (op == 'M')// run it twice it it's a M (S and L)
	       {
		  par = sim_c(myCache, par, addr, op, size);
		  par = sim_c(myCache, par, addr, op, size);
	       }
	       if (op == 'S')// run it once for S
		  par = sim_c(myCache, par, addr, op, size);
	    }
	 }
	 
      }
      
   }

   //if there is a -h/-v
   else if (argc == 10)
   {
      if (strcmp(argv[1], "-h") == 0)
      {
	 hel();
	 return 0;
      }

      if(strcmp(argv[1], "-v") == 0)
      {
	 // shows all the misses/hits/evicts when the program is run
	 // sets the tag which is a global so when it is read by the
	 // sim_c function it outputs the correct line
	 verbose = 1;
	 if (strcmp(argv[2], "-s") == 1)
	 {
	    hel();
	    return 0;
	 }
      }
      // same as when there are 9 arguements except with index for the
      // flags is different
      if(strcmp(argv[2], "-s") == 0)
      {
	 par.s = atoi (argv[3]);// get the string index from the input
      }
      
      if(strcmp(argv[4], "-E") == 0)
      {
	 par.E = atoi (argv[5]);
      }

      if(strcmp(argv[6], "-b") == 0)
      {
	 par.b = atoi (argv[7]);
	 
      }

      //compute S and B
      par.S = pow(2.0, par.s);
      par.B = pow(2.0, par.b);

      par.hits = 0;
      par.miss = 0;
      par.evict = 0;
      
      myCache = b_c(par.S, par.E, par.B);
      //get the trace file and run the simulator
      if (strcmp(argv[8], "-t") == 0)
      {
	 f_read = fopen(argv[9], "r"); // open the trace file and set it so we can read it
	 //-----------------------------------------------------------------------------------------
	 if (!f_read)// if we can't read the file
	 {
	    printf("file isn't there yo\n");
	 }
	 
	 else
	 {
	    
	    while (fscanf(f_read, " %c %llx,%d", &op, &addr, &size) == 3)// while there are 3 inputs that fit op, addr and size
	    {
	       if (op == 'I')
	       {
	       }
	       if (op == 'L')
	       {
		  par = sim_c(myCache, par, addr, op, size);
	       }
	       if (op == 'M')
	       {
		  par = sim_c(myCache, par, addr, op, size);
		  par = sim_c(myCache, par, addr, op, size);
	       }
	       if (op == 'S')
		  par = sim_c(myCache, par, addr, op, size);
	    }
	 }//end of else
	 
      }// end of if statement for -t
   }// end of while in else
   else
   {
      if (strcmp(argv[1], "-h") == 0)
      {
	 hel();
	 return 0;
      }
      else
      {
	 printf("./csim: Missing required command line argument\n");
	 hel();
	 return 0;
      }
   }
   
   printSummary(par.hits, par.miss, par.evict);

   for (int i = 0; i < par.S; i++)// clear all the mallocs
   {
      set_c s = myCache.sets[i];
      if (s.lines!= NULL)
	 free(s.lines);
   }
   if (myCache.sets != NULL)
      free(myCache.sets);
   
   fclose(f_read);
   return 0;
}
