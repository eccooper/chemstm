/*
    SPARTAN PARSER

    Extracts all surfaces output by Spartan when using
    the DUMPVOLUME option in calculations.

    INPUT FORMATS

    Spartan Output .txt file(s) with or without extension

    OUTPUT FORMATS

    USAGE

*/

#include <stdio.h>
#include <stdlib.h>

#include "spartan.h"

int main(int argc, char** argv){

    if(argc <= 1){
        printf("\nPlease provide file(s) for parsing\n\n\tExample: sparse.exe tryp1.txt benzene.txt\n");
        return -1;
    }

    // BE CAREFUL HERE! (this works on windows and linux)
    printf("\nChecking for output directory:\n");
    system("mkdir sparse_output");

    // parse through each file and save our formatted output
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
        output_parsed_file(so, SO_DEFAULT);
        //output_parsed_file_clean(so);
        close_spartan_file(so);
    }
    return 0;
}
