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
   int s; // the cache sets
   int b; // block size
   int E; // number of cache lines in a set
   int S; // num of sets S = 2**s
   int B; // B = 2 ** b

   int hits;
   int miss;
   int evict;
   
} param;

//a struct for each set line
typedef struct
{
   int valid; // valid bit
   char *block;  //block
   unsigned long long tag; // tag bit
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


   // allocate storage for the sets in cache
   new_c.sets = (set_s *) malloc(sizeof(set_s) * set_n);

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

//LRU function cause it's better than jaming everything in sim cache func
int LRU (set_s c, param par, int *used_l)
{

   //most and least recently used lines
   int most_used = c.lines[0].last_line; // max used
   int least_used = c.lines[0].last_line; // min used

   int min_ind = 0; //the LRU that will be returned
   
   set_l line;

   // search through all the lines, if the current min is > the number
   // of times the line is accessed store the index and return it
   for (int i = 1; i < par.E; i++)
   {
      line = c.lines[i];

      if (least_used > line.last_line)
      {
	 min_ind = i;
	 least_used = line.last_line;
      }

      if (most_used < line.last_line)
      {
	 most_used = line.last_line;
      }
   }

   used_l[0] = least_used;
   used_l[1] = most_used;

   return min_ind;
}

//simulate the cache
param sim_c (cache c, param par, long long int addr)
{
   int c_full = 1; // is the cache full?

   int lines_n = par.E;
   int prev = par.hits;

   unsigned long long temp = addr << (64 - (par.s + par.b)); // bit shift left the tag size
   unsigned long long set_ind = temp >> ((64 - (par.s+par.b)) + par.b); // set index is bit shifted right

   unsigned long long in_tag = addr >> (par.s+par.b);

   set_s q_set = c.sets[set_ind];

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
	 }
      }
      else if(!(line.valid && c_full))
	 c_full = 0;
      
   }
   if (prev == par.hits)
      //miss
      par.miss++;
   else
      return par;// we found the data in the cache and it was a hit

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
   else //else evict
   {
      par.evict++;
      q_set.lines[min_ind].tag = in_tag;
      q_set.lines[min_ind].last_line = used_l[1]+1;
   }
   free(used_l);
   return par;
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
   
   cache my_cache;
   param par;
   
   char op; // takes the operation
   int size;
   unsigned long long int addr;
   //file type variable to open and read the file
   FILE *f_read;
   
   //if there is no -h/-v/-hv
   if (argc == 9)
   {
      printf("argc: %d", argc);
      
      if(strcmp(argv[1], "-s") == 0)
      {
	 printf("\nin -s \n");
	 par.s = atoi (argv[2]);// get the string index from the input

         //test
	 printf ("%d", par.s);
	 printf("\n");
      }

      if(strcmp(argv[3], "-E") == 0)
      {
	 printf ("\nin -E\n");
	 par.E = atoi (argv[4]);
	 
	 //test
	 printf("%d", par.E);
	 printf("\n");
      }

      if(strcmp(argv[5], "-b") == 0)
      {
	 printf ("\nin -b\n");
	 par.b = atoi (argv[6]);
	 
	 //test
	 printf("%d", par.b);
	 printf("\n");
      }
      
      //get the trace file and run the simulator
      if (strcmp(argv[7], "-t") == 0)
      {
	 f_read = fopen(argv[8], "r"); // open the trace file and set it so we can read it
	 //-----------------------------------------------------------------------------------------
	 if (!f_read)// if we can't read the file
	 {
	    printf("file isn't there yo\n");
	 }
	 
	 else
	 {
	    while (fscanf(f_read, "%c %llx,%d", &op, &addr, &size) == 3)// while there are 3 inputs that fit op, addr and size
	    {
	       if (op == 'I')
	       {
	       }
	       if (op == 'L')
		  par = sim_c(my_cache, par, addr);
	       if (op == 'M')
	       {
		  par = sim_c(my_cache, par, addr);
		  par = sim_c(my_cache, par, addr);
	       }
	       if (op == 'S')
		  par = sim_c(my_cache, par, addr);
	       else
		  printf("there is an error in the trace file");
	    }
	 }//end of else
	 
      }// end of if statement for -t
      
   }//end of if statement for if (argc == 9)
/*
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
*/
   printSummary(par.hits, par.miss, par.evict);
   fclose(f_read);
   return 0;
}
