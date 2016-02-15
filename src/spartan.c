#include "spartan.h"


SpartanOutput* open_spartan_file(char* filename){
    SpartanOutput* so = malloc(sizeof(SpartanOutput));

    so->file = fopen(filename, "r");
    strncpy(so->filename, filename, SOUT_FILENAME_LENGTH);

    //printf("\nOpening: %s\n", so->filename);
    so->numsurf = 0;

    return so;
}


void parse_spartan_file(SpartanOutput* so, int flags){
    int log = 0;

    FILE*   file = so->file;
    char    line[SOUT_LINE_LENGTH];
    int     state = PS_GFXDUMP;
    int     surfcount = 0;
    int     lastmesh = 0;

    if(flags & SF_LOG_INFO){
        printf("\nLOGGING ENABLED\n");
        log = 1;
    }

    // we will parse surface output using a simple state machine to
    // decide what we are looking for as the file is streamed line by line
    while(fgets(line, SOUT_LINE_LENGTH, file) != NULL){
        //printf("Line: %s\n", line);

        // first we check for "---- Begin Graphics Surface Dump"
        if(state == PS_GFXDUMP){
            if(strncmp(line, "---- Begin Graphics Surface Dump", 32) == 0){
                //so->numsurf = ++surfcount;
                if(log){
                    printf("\n----------------------------------------");
                    printf("\nSURFACE #%d\n", surfcount);
                }
                // setup a new surface
                start_new_surface(so);
                // next look for isovalue entry
                state = PS_ISOVAL;
            }
        // checking for isovalue header, string starts with "title :"
        } else if(state == PS_ISOVAL){
            if(strncmp(line, "title :", 7) == 0){
                if(log) printf("\nSurface Header\n");

                char* token = strtok(line, " :=");
                int namefound, ivfound = 0;

                while(token != NULL){
                    //printf("\tToken: %s\n", token);

                    if(namefound){
                        if(log) printf("\tName: %s\n", token);
                        namefound = 0;
                        strcpy(so->surf[so->numsurf-1]->name, token);
                    } else if(ivfound){
                        if(log) printf("\tIsovalue: %s\n", token);
                        ivfound = 0;
                        so->surf[so->numsurf-1]->isovalue = strtof(token, NULL);
                    } else if(strncmp(token, "surface", 7) == 0){
                        namefound = 1;
                    } else if(strncmp(token, "value", 5) == 0){
                        ivfound = 1;
                    }

                    token = strtok(NULL, " :=");
                }
                state = PS_POSMESH;
            }
        // check for mesh tag "Positive Mesh" or "Negative Mesh"
        } else if(state == PS_POSMESH || state == PS_NEGMESH){
            if(state == PS_POSMESH){
                // pos mesh check
                if(strncmp(line, " Positive Mesh", 14) == 0){
                    if(log) printf("\nPositive Mesh\n");
                    state = PS_VERTEX;
                    lastmesh = 1; // currently parse a positive mesh
                    start_surface_mesh(so, 1); // create container for pos data
                }
            } else {
                // neg mesh check
                if(strncmp(line, " Negative Mesh", 14) == 0){
                    if(log) printf("\nNegative Mesh\n");
                    state = PS_VERTEX;
                    lastmesh = -1; // currently checking a negative mesh
                    start_surface_mesh(so, -1); // create container for meg data
                }
            }
        // now we read vertices until a line that ends the section
        } else {
            // lines at the end are either empty or contain specific text
            if(strncmp(line, "\n", 1) == 0 ||
               strstr(line, "MO") != NULL ||
               strstr(line, "Surface") != NULL){
                int vert_count;
                if(lastmesh > 0){
                    vert_count = (so->surf[so->numsurf-1]->num_pos)/3;
                    state = PS_NEGMESH;
                } else {
                    vert_count = (so->surf[so->numsurf-1]->num_neg)/3;
                    state = PS_GFXDUMP;
                }
                if(log) printf("\tRead %d vertices\n", vert_count);
            } else if (state == PS_VERTEX){ // still looking for a vertex?
                char* token = strtok(line, " ");
                int tokencount = 0;
                // initialize as obviously wrong values to make debug easier
                float x = -9999.9999;
                float y = -9999.9999;
                float z = -9999.9999;
                while(token != NULL){
                    // expecting tokens 4-6 to be floating point values
                    if(tokencount >= 3 && tokencount < 6){
                        //if(log) printf("\tToken: %s\n", token);
                        if(tokencount == 3) x = strtof(token, NULL);
                        else if(tokencount == 4) y = strtof(token, NULL);
                        else if(tokencount == 5) z = strtof(token, NULL);
                    }
                    token = strtok(NULL, " ");
                    tokencount++;
                }
                // add the vertex to the next available position in our surface
                if(log) printf("Vert: %f %f %f\n", x, y, z);
                add_surface_vert(so, x, y, z);
            }
        }
    }

    if(log) printf("\n----------------------------------------\n");
}

void output_parsed_file(SpartanOutput* so, int flags){
    // quick and dirty output for testing, fairly poor code
    if(flags == SO_DEFAULT){
        printf("\n");

        char* title = strtok(so->filename, ".");
        char* dir = "sparse_output/";

        char posfn[SOUT_FILENAME_LENGTH];
        char negfn[SOUT_FILENAME_LENGTH];

        strcpy(posfn, dir);
        strcpy(negfn, dir);

        strcat(posfn, title);
        strcat(negfn, title);

        strcat(posfn, "_raw_pos.txt");
        strcat(negfn, "_raw_neg.txt");

        //printf("Files: %s and %s", posfn, negfn);

        FILE* posfile = fopen(posfn, "w+");
        FILE* negfile = fopen(negfn, "w+");
        for(int i=0; i<so->numsurf; i++){
            for(int j=0; j<so->surf[i]->num_pos; j+=3){
                fprintf(posfile, "%.4f %.4f %.4f %.4f\n", so->surf[i]->pos_verts[j], so->surf[i]->pos_verts[j+1], so->surf[i]->pos_verts[j+2], so->surf[i]->isovalue);
            }
            printf("Wrote %d verts for isovalue %.3f (+) orbital\n", so->surf[i]->num_pos/3, so->surf[i]->isovalue);
        }

        fclose(posfile);

        printf("\n");

        for(int i=0; i<so->numsurf; i++){
            for(int j=0; j<so->surf[i]->num_neg; j+=3){
                fprintf(negfile, "%.4f %.4f %.4f %.4f\n", so->surf[i]->neg_verts[j], so->surf[i]->neg_verts[j+1], so->surf[i]->neg_verts[j+2], so->surf[i]->isovalue);
            }
            printf("Wrote %d verts for isovalue %.3f (-) orbital\n", so->surf[i]->num_neg/3, so->surf[i]->isovalue);
        }
        fclose(negfile);
    }
}


void close_spartan_file(SpartanOutput* so){
    printf("\nCleaning surfaces:\n\n");
    for(int i=0; i<so->numsurf; i++){
        printf("\tCLOSING: %s %f %d %d\n",
            so->surf[i]->name, so->surf[i]->isovalue,
            so->surf[i]->num_pos/3, so->surf[i]->num_neg/3);
        free(so->surf[i]->pos_verts);
        free(so->surf[i]->neg_verts);
        free(so->surf[i]);
    }

    if(so->file != NULL)
        fclose(so->file);

    free(so);
}

void start_new_surface(SpartanOutput* so){
    so->surf[so->numsurf] = malloc(sizeof(SpartanSurface));
    so->numsurf++;
}

void set_surface_header(SpartanOutput* so, char* name, float isoval){
    if(so->numsurf <= 0){
        printf("ERROR: No surfaces have been added yet!\n");
        exit(-1);
    }
    SpartanSurface* surf = so->surf[so->numsurf-1];

    strcpy(surf->name, name);
    surf->isovalue = isoval;
}

void start_surface_mesh(SpartanOutput* so, int type){
    if(so->numsurf <= 0){
        printf("ERROR: No surfaces have been added yet!\n");
        exit(-1);
    }

    // initialize arrays for the positive and negative vertices
    SpartanSurface* surf = so->surf[so->numsurf-1];
    if(type > 0){
        //printf("Positive Vertex Array\n");
        surf->pos_verts = (float*)malloc(sizeof(float)*SSURF_INIT_VERTS);
        surf->target = 1;
        surf->num_pos = 0;
        surf->max_pos = SSURF_INIT_VERTS;
    } else {
        //printf("Negative Vertex Array\n");
        surf->neg_verts = (float*)malloc(sizeof(float)*SSURF_INIT_VERTS);
        surf->target = -1;
        surf->num_neg = 0;
        surf->max_neg = SSURF_INIT_VERTS;
    }
}

void add_surface_vert(SpartanOutput* so, float x, float y, float z){
    if(so->numsurf <= 0){
        printf("ERROR: No surfaces have been added yet!\n");
        exit(-1);
    }

    SpartanSurface* surf = so->surf[so->numsurf-1];
    int target = surf->target;

    if(surf->pos_verts == NULL && surf->neg_verts == NULL){
        printf("ERROR: No vertex arrays have been defined for this surface!\n");
        exit(-1);
    }

    // the vertex arrays are dynamic, we need to check if they are full first
    float* verts;
    int    vert_count, vert_max;

    if(target > 0){
        verts = surf->pos_verts;
        vert_count = surf->num_pos;
        vert_max = surf->max_pos;
    } else {
        verts = surf->neg_verts;
        vert_count = surf->num_neg;
        vert_max = surf->max_neg;
    }

    if((vert_count+3) >= (vert_max-1)){
        vert_max = vert_max*2;
        if(target > 0){
            surf->max_pos = vert_max;
        } else {
            surf->max_neg = vert_max;
        }
        float* new_verts = (float*)realloc(verts, sizeof(float)*vert_max);
        if(new_verts != NULL){
            verts = new_verts;
            if(target > 0){
                surf->pos_verts = verts;
            } else {
                surf->neg_verts = verts;
            }
        } else {
            printf("ERROR: Failed to allocate new memory block!\n");
            exit(-1);
        }
    }

    // ok should be safe to add in new vertices
    verts[vert_count] = x;
    verts[vert_count+1] = y;
    verts[vert_count+2] = z;

    vert_count += 3;

    //printf("Vert: %f %f %f (%d, %d)\n", x, y, z, target, vert_count);

    if(target > 0){
        surf->num_pos = vert_count;
    } else {
        surf->num_neg = vert_count;
    }
}