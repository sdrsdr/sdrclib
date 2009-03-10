
#include <stdlib.h>
#include "str_split.h"

//---------------------------------------------------------------------------
// char *src  -> Source to split
// char *starts[maxstarts] -> tokens starts are stored here (this points to src's memory)
// int  lengths[maxstarts] -> tokens lengthss are stored here
// maxstarts -> zizeof (starts)=zizeof (lengths)
// sep -> tokens separator
// contsep -> treat two-or-more separator as one separator OR add a   starts[current]=NULL for every two separators
// return -> num of tokens found

int split_str (char *src, char* *starts,int *lengths, int maxstarts, char sep, int contsep) {
	int nt=1; 
	int mode=0;
	char *cc=src;
	char c;
 
	if (!src) return 0;
	if (*src==0) return 0;
	if (!maxstarts) return 0;
	*starts=cc;
	*lengths=0;
	while ((c=*cc)!=0) {
		if (mode==0 && c==sep) { //was text now sep
			mode=1;
			if (*lengths==0) { //separator begining
				*starts=NULL;
				if (nt==maxstarts) break; //we can't enter mode 2 in separator
			}
		} else if (mode==1 && c==sep && !contsep) { //was sep now sep and not continueing separator: output an empty token
			starts++; nt++;
			(*starts)=NULL;
			lengths++;
			(*lengths)=0;
			if (nt==maxstarts) break; //we can't enter mode 2 in separator
		} else if (mode==1 && c!=sep) { //was sep now text
			starts++; nt++;
			(*starts)=cc;
			lengths++;
			(*lengths)=1;
			if (nt==maxstarts) mode=2; //we enter mode 2 so laste elem contains all the remaining chars
			else mode=0;
		} else { // was text now text or serching for eos
			(*lengths)++;
		}
		cc++; //advance;
	}
	if (nt==maxstarts) return nt;
	if (mode==1) {
		starts++; nt++;
		(*starts)=NULL;
		lengths++;
		(*lengths)=0;
	};
	return nt;
}
