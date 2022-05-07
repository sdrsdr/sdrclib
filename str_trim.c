
#include <stdlib.h>
#include "str_trim.h"

/* Part of sdrclib from https://github.com/sdrsdr/sdrclib */

char *str_trim(char *start, int *len, char trim1, char trim2, char trim3){
	char *endp=start+(*len)-1;
	char c=0;
	
	//ltrim:
	while (start<=endp) {
		c=*start;
		if (c==0) break;
		if (c==trim1 || c==trim2 || c==trim3){
			start++; (*len)--;
			continue;
		}
		break;
	}
	if (c==0 || *len==0) return start;
	//rtrim
	while (start<=endp) {
		c=*endp;
		if (c==trim1 || c==trim2 || c==trim3){
			endp--; (*len)--;
			continue;
		}
		break;
	}
	return start;
}
