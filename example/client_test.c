#include "../include/libmpn.h"

int main(){
	init_mpn_client("localhost", 8090);


	while(1){
		scan_for_incomming_packets(0);
	}
}
