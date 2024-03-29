//------------------------------------------------------------------------
// Filename:
//    smoker.cpp
// PROGRAM DESCRIPTION
//    Implementation of smoker threads and agent thread
//------------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "smoker.h"

// local macro defines

#define MATCH_PAPER         0
#define PAPER_TOBACCO       1
#define TOBACCO_MATCH       2

// global data variables

static char *materials[]={ "paper", "match", "tobacco"};
static char *names[]  =  { "Smoker(Tobacco)", "Smoker(Paper)", "Smoker(Match)"};

// semaphores
static Semaphore PaperMatch("PaperMatch");
static Semaphore MatchTobacco("MatchTobacco");
static Semaphore TobaccoPaper("TobaccoPaper");
static Semaphore *Sem[NUM_OF_SMOKERS] = {&PaperMatch, &MatchTobacco, &TobaccoPaper};
static Semaphore Table("Table", 0);// control the sharing of the table

// -----------------------------------------------------------------------
// FUNCTION  Filler():
//    This function fills a strstream with spaces.
// -----------------------------------------------------------------------

strstream *Filler(int n)
{
     int  i;
     strstream *Space;

     Space = new strstream;
     for (i = 0; i < n; i++)
        (*Space) << ' ';
     (*Space) << '\0';
     return Space;
}

//------------------------------------------------------------------------
// Smokerthread::Smokerthread()
//      constructor for Smokerthread class
//------------------------------------------------------------------------

Smokerthread::Smokerthread(int Number)
{
     No = Number;
     ThreadName.seekp(0, ios::beg);
     if (Number == 0)
        ThreadName << "Smoker(Tobacco)" << '\0';
     else if (Number == 1)
          ThreadName << "Smoker(Paper)" << '\0';
     else
          ThreadName << "Smoker(Match)" << '\0';
}

//------------------------------------------------------------------------
// Smokerthread::ThreadFunc()
//      implement a smoker thread
//------------------------------------------------------------------------

void Smokerthread::ThreadFunc()
{
     Thread::ThreadFunc();
     strstream *Space;

     Space = Filler(4);            // build leading spaces
     while (1) {
          Sem[No]->Wait();         // smoker waits for materials
          cout << Space->str() << ThreadName.str() << " is smoking" << endl;
          Table.Signal();          // take ingredients and release the table
          Delay();                 // smoking for a while
     }
}

//------------------------------------------------------------------------
// Agentthread::Agentthread()
//      constructor for Agentthread class
//------------------------------------------------------------------------

Agentthread::Agentthread(char *ThreadName, int iter)
            : Thread(ThreadName)
{
     Iterations = iter;
     srand((unsigned int) time(NULL));
}

//------------------------------------------------------------------------
// Agentthread::ThreadFunc()
//      implement a agent thread
//------------------------------------------------------------------------

void Agentthread::ThreadFunc()
{
     Thread::ThreadFunc();
     int RandomNo;

     for (int i = 0; i < Iterations; i++) {
          Delay();
          RandomNo = rand() % NUM_OF_SMOKERS;
          cout << "Agent prepares " << materials[RandomNo] << " and "
               << materials[(RandomNo+1) % NUM_OF_SMOKERS] << endl;
          Sem[RandomNo]->Signal();        // randomly provide material to one smoker
          Table.Wait();                   // waiting for smoker finish smoking
          cout << "Agent finishes serving "  << names[RandomNo] << endl;
     }
     cout << "Agent quits." << endl;
     Exit();
}

// end of smoker.cpp