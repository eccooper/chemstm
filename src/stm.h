#pragma once

/*
	Initial draft of STM simulator based on
	previous work by Raymond Terryn
*/

#include "spartan.h"


#define STMRES_NAME_LENGTH 			256
#define STMRES_FILENAME_LENGTH 		256

#define DEFAULT_STM_TIP_RAD 		19
#define DEFAULT_UNIT_SIZE 			0.125f

typedef struct STMResult STMResult;


struct STMResult {
	float 	unit_size;
	int 	tip_radius;

	float* 	surface;
	int 	surf_w;
	int 	surf_h;
	int 	surf_d;

	float*  conv_result;

	char 	name[STMRES_NAME_LENGTH];
};

STMResult* 	start_simulation();
void 		generate_stm_tip(STMResult* res, int radius);
void 		simulate_spartan_file(STMResult* result, SpartanOutput* so);
void 		close_simulation(STMResult* result);

void 		output_stm_result(STMResult* result);

float* 		interp_delaunay(float* pointset, int pointset_size, float unit_size);