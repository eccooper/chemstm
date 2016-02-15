#pragma once

/*
    Data types and functions for reading output from Spartan

    Writing a new output format just involves extending or adding
    another output_parsed_file() function and using the
    data structures we provide.
*/

/*
    TODO: REMOVE NEGATIVE Z VALUES (BELOW THE PLANE)
    TODO: REMOVE DUPLICATE COORDINATES (IN OUTPUT)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// CONSTANTS
#define SOUT_FILENAME_LENGTH    256
#define SOUT_MAX_SURFACES       256
#define SOUT_LINE_LENGTH        1024

#define SSURF_NAME_LENGTH       256
#define SSURF_INIT_VERTS        2048

// DATA TYPES
typedef struct  SpartanOutput       SpartanOutput;
typedef enum    SpartanParseFlags   SpartanParseFlags;
typedef enum    SpartanParseState   SpartanParseState;
typedef enum    SpartanOutputFlags  SpartanOutputFlags;

typedef struct  SpartanSurface      SpartanSurface;

// the header for a 'Spartan Output' text file
struct SpartanOutput {
    FILE*   file;
    int     fsize; // in kB
    char    filename[SOUT_FILENAME_LENGTH];

    int                 numsurf;
    SpartanSurface*     surf[SOUT_MAX_SURFACES];
};

// flags for controlling the parser
enum SpartanParseFlags {
    SF_DEFAULT =    1 << 0,
    SF_LOG_INFO =   1 << 1
};

// flags for parser state machine
enum SpartanParseState {
    PS_GFXDUMP,
    PS_ISOVAL,
    PS_POSMESH,
    PS_NEGMESH,
    PS_VERTEX
};

// optional controls for outputting formatted data
enum SpartanOutputFlags {
    SO_DEFAULT =    1 << 0
};

// surfaces contain vertices and the isovalue representing the shape of a MO
struct SpartanSurface {
    char        name[SSURF_NAME_LENGTH];
    float       isovalue;

    int         target;
    int         max_pos, max_neg;
    int         num_pos, num_neg;
    float*      pos_verts;
    float*      neg_verts;
};

// PARSING API

// open file from disk for streaming and prepare data
SpartanOutput*      open_spartan_file(char* filename);

// parse a spartan output file into our intermediate structures
void                parse_spartan_file(SpartanOutput* so, int flags);

// output parsed data in a suitable format
void                output_parsed_file(SpartanOutput* so, int flags);

// responsible for cleaning up the mess after we are done
void                close_spartan_file(SpartanOutput* so);

// functions for assembling spartan surfaces
void                start_new_surface(SpartanOutput* so);
void                set_surface_header(SpartanOutput* so, char* name, float isoval);
void                start_surface_mesh(SpartanOutput* so, int type);
void                add_surface_vert(SpartanOutput* so, float x, float y, float z);