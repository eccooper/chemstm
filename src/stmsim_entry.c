#include "stm.h"

#define STMSIM_VER 0.1

int main(int argc, char** argv){
	printf("STM Simulation %.1f\n", STMSIM_VER);

	if(argc <= 1){
        printf("\nPlease provide file(s) for simulation\n\n\tExample: stmsim.exe tryp1.txt benzene.txt\n");
        return -1;
    }

    // BE CAREFUL HERE! (this works on windows and linux)
    printf("\nChecking for output directory:\n");
    system("mkdir stmsim_output");

    for(int i=1; i<argc; i++){
        printf("\nInput:\n");
        printf("\t(%d): %s\n", i, argv[i]);

        SpartanOutput* so = open_spartan_file(argv[i]);

        // if this file doesn't exist, go ahead and try to parse the others
        if(so->file == NULL){
            printf("\nFailed to open file: %s\n", argv[i]);
            close_spartan_file(so);
            continue;
        }

        // see spartan.c for implementations
        parse_spartan_file(so, SF_DEFAULT);

        // run parsed output through the simulator
        STMResult* res = start_simulation();
        simulate_spartan_file(res, so);
        output_stm_result(res);
        
        //free up memory
        close_spartan_file(so);
        close_simulation(res);
    }

	return 0;
}