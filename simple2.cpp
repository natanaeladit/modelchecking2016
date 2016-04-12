#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include <sylvan.h>
#include <sylvan_obj.hpp>

#include <iostream>

using namespace sylvan;

Bdd BDDEX(Bdd B1, Bdd B2);
Bdd BDDEU(Bdd B1, Bdd B2, Bdd oldLfp);
void print(Bdd b);
void generateGraph(Bdd b);

VOID_TASK_0(simple_cxx)
{   
    Bdd x = Bdd::bddVar(0);
    Bdd x_ = Bdd::bddVar(1);
    Bdd y = Bdd::bddVar(2);
    Bdd y_ = Bdd::bddVar(3);
    Bdd z = Bdd::bddVar(4);
	Bdd z_ = Bdd::bddVar(5);

	Bdd one = Bdd::bddOne(); // the True terminal
    Bdd zero = Bdd::bddZero(); // the False terminal

	Bdd R1 = !x * x_ * y *= y_ * z *= z_; 
	Bdd R2 = x * x_ * !y * y_ * z *= z_; 
	Bdd R3 = x *= x_ * y *= y_ * z *= !z_; 
	Bdd R = R1 + R2 + R3;

	print(R1);
	generateGraph(R1);

	Bdd initState = !x * !y * !z;
	Bdd finalState = x * y * z;
	Bdd errorState = !x * y * !z;
	Bdd payState = y *= !z;
	
	/*
	Reachability Analysis : AG finalState -> !EF (!finalState) -> !E (true U !finalState)

	Formula : 
	EF finalState
	E (true U finalState)                          => this is Z
	finalState + (true * EX E (true U finalState))
	finalState + (true * EX Z)*/

	Bdd Z0 = Bdd::bddZero(); // False
	Bdd Prev_Z0_R = Z0 * R;
	Bdd Z1 = finalState + Prev_Z0_R;
	Bdd Prev_Z1_R = Z1 * R;
	Bdd Z2 = finalState + Prev_Z1_R;
	Bdd Prev_Z2_R = Z2 * R;
	Bdd Z3 = finalState + Prev_Z2_R;

	//Bdd result = sylvan_equiv(Z3.GetBDD(), Z2.GetBDD());
	
	printf("%d\n", Z2 == Z3);	// True -> the formula holds


	//Bdd test = sylvan_exists(R.GetBDD(),finalState.GetBDD());

	//Bdd test = sylvan_relprev(finalState.GetBDD(), R.GetBDD(), false);
	
	//print(test);

	//generateGraph(R);
	
	std::cerr << "Start MD" << std::endl;
    //Bdd result = BDDEX(R, finalState); 
	Bdd result = BDDEU(R, finalState, Bdd::bddZero());
	std::cerr << "END MD" << std::endl;
	
	std::cerr << "Printing" << std::endl;
	print(result);
	generateGraph(result);
	std::cerr << "END" << std::endl;
}

Bdd BDDEU(Bdd B1, Bdd B2, Bdd oldLfp){	
    Bdd lfp = B2 + B1 * oldLfp;

    if(lfp == oldLfp){
        return lfp;
    }
    
    return BDDEU(B1, B2, lfp);
}

void print(Bdd b){
    if(!b.isTerminal()){
        std::cerr << "NODE:  "<< b.TopVar()  << std::endl;    
        
        print(b.Then());
        print(b.Else());
    }else{
        std::cerr << "TERMINAL:  "<< (b==Bdd::bddOne()) << (b==Bdd::bddZero())  << std::endl;
    }
}

void generateGraph(Bdd b){
	FILE * pFile;
	pFile = fopen ("simple.dot" , "w");
	if (pFile == NULL) 
		perror ("Error opening file");
	else
	{
		sylvan_fprintdot(pFile,b.GetBDD());
	}
	fclose (pFile);
}

VOID_TASK_1(_main, void*, arg)
{
    // Initialize Sylvan
    // With starting size of the nodes table 1 << 21, and maximum size 1 << 27.
    // With starting size of the cache table 1 << 20, and maximum size 1 << 20.
    // Memory usage: 24 bytes per node, and 36 bytes per cache bucket
    // - 1<<24 nodes: 384 MB
    // - 1<<25 nodes: 768 MB
    // - 1<<26 nodes: 1536 MB
    // - 1<<27 nodes: 3072 MB
    // - 1<<24 cache: 576 MB
    // - 1<<25 cache: 1152 MB
    // - 1<<26 cache: 2304 MB
    // - 1<<27 cache: 4608 MB
    sylvan_init_package(1LL<<22, 1LL<<26, 1LL<<22, 1LL<<26);

    // Initialize the BDD module with granularity 1 (cache every operation)
    // A higher granularity (e.g. 6) often results in better performance in practice
    sylvan_init_bdd(1);

    // Now we can do some simple stuff using the C++ objects.
    CALL(simple_cxx);

    // Report statistics (if SYLVAN_STATS is 1 in the configuration)
    sylvan_stats_report(stdout, 1);

    // And quit, freeing memory
    sylvan_quit();

    // We didn't use arg
    (void)arg;
}

int main (int argc, char *argv[])
{
    int n_workers = 0; // automatically detect number of workers
    size_t deque_size = 0; // default value for the size of task deques for the workers
    size_t program_stack_size = 0; // default value for the program stack of each pthread

    // Initialize the Lace framework for <n_workers> workers.
    lace_init(n_workers, deque_size);

    // Spawn and start all worker pthreads; suspends current thread until done.
    lace_startup(program_stack_size, TASK(_main), NULL);

    // The lace_startup command also exits Lace after _main is completed.

    return 0;
    (void)argc; // unused variable
    (void)argv; // unused variable
}
