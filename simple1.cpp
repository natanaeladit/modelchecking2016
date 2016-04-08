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
    Bdd v0 = Bdd::bddVar(0);
    Bdd v1 = Bdd::bddVar(1);
    Bdd v2 = Bdd::bddVar(2);
    Bdd v3 = Bdd::bddVar(3);
	
    Bdd v4 = Bdd::bddVar(4);
    Bdd v5 = Bdd::bddVar(5);
	Bdd v6 = Bdd::bddVar(6);
	Bdd v7 = Bdd::bddVar(7);

    
    Bdd k0 = Bdd::bddVar(8);
    Bdd k1 = Bdd::bddVar(9);
    Bdd k2 = Bdd::bddVar(10);
    Bdd k3 = Bdd::bddVar(11);
	
    Bdd k4 = Bdd::bddVar(12);
    Bdd k5 = Bdd::bddVar(13);
	Bdd k6 = Bdd::bddVar(14);
	Bdd k7 = Bdd::bddVar(15);
    
	
	Bdd R1 = !v0*!k0*!v1*k1 *!v2=!k2*v3 =k3		*  v4=k4*!v5=!k5*!v6=!k6*v7=k7; 
	Bdd R2 = !v0*k0 *v1 *!k1*!v2=!k2*v3 =k3		*  v4*k4*!v5*k5 *!v6=!k6*v7=k7; 
	Bdd R3 = v0 =k0 *!v1=!k1*!v2*!k2*v3 *!k3	*  v4=k4*v5 =k5 *!v6=!k6*v7=k7; 
	Bdd R4 = v0 *k0 *!v1*k1 *!v2=!k2*!v3=!k3	*  v4=k4*v5 =k5 *!v6=!k6*v7=k7; 
	Bdd R5 = v0 =k0 *v1 =k1 *!v2*!k2*!v3*k3		*  v4=k4*v5 =k5 *!v6*k6 *v7*!k7;

	Bdd R = R1 + R2 + R3 + R4 + R5;
	
	Bdd initState = v4*!v5*!v6*v7;

	Bdd finalState = v4*v5*v6*!v7;
	
	/*
	Formula : 
	EF finalState
	E (true U finalState)                          => this is Z
	finalState + (true * EX E (true U finalState))
	finalState + (true * EX Z)
	*/

	Bdd Z0 = Bdd::bddZero(); // False
	Bdd Z1 = finalState;
	Bdd Prev_Z1_R = sylvan_relprev(Z1.GetBDD(), R.GetBDD(), false);
	Bdd Z2 = finalState + Prev_Z1_R;
	Bdd Prev_Z2_R = sylvan_relprev(Z2.GetBDD(), R.GetBDD(), false);
	Bdd Z3 = finalState + Prev_Z2_R;
	Bdd Prev_Z3_R = sylvan_relprev(Z3.GetBDD(), R.GetBDD(), false);
	Bdd Z4 = finalState + Prev_Z3_R;	
	Bdd Prev_Z4_R = sylvan_relprev(Z4.GetBDD(), R.GetBDD(), false);
	Bdd Z5 = finalState + Prev_Z4_R;

	Bdd result = sylvan_equiv(Z4.GetBDD(), Z5.GetBDD());

	printf("%d\n", Z4 == Z5);	// True -> the formula holds

	//print(result);

	//generateGraph(result);

	//Bdd test = sylvan_exists(R.GetBDD(),finalState.GetBDD());

	//Bdd test = sylvan_relprev(finalState.GetBDD(), R.GetBDD(), false);
	
	//print(test);

	//generateGraph(test);
	
	//std::cerr << "Start MD" << std::endl;
    //Bdd result = BDDEX(R, finalState); 
	//Bdd result = BDDEU(R, finalState, Bdd::bddZero());
	//std::cerr << "END MD" << std::endl;
	
	//std::cerr << "Printing" << std::endl;
	//print(result);
	//std::cerr << "END" << std::endl;
}

Bdd BDDEX(Bdd B1, Bdd B2){
	Bdd ex = B1.ExistAbstract(B2);
	//Bdd ex = sylvan_relprev(B1.GetBDD(), B2.GetBDD(), false);
    return ex;
}  

Bdd BDDEU(Bdd B1, Bdd B2, Bdd oldLfp){	
    Bdd lfp = B2 + B1 * BDDEX(B1, B2);

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
