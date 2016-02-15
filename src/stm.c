#include "stm.h"

STMResult* start_simulation(){
	STMResult* result = malloc(sizeof(STMResult));
	result->unit_size = DEFAULT_UNIT_SIZE;
	generate_stm_tip(result, DEFAULT_STM_TIP_RAD);
	return result;
}

void generate_stm_tip(STMResult* result, int radius){

}

void simulate_spartan_file(STMResult* result, SpartanOutput* so){

}

void output_stm_result(STMResult* result){

}

void close_simulation(STMResult* result){
	
}