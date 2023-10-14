
#include <stdlib.h>
#include "str_split.h"

/* Part of sdrclib from https://github.com/sdrsdr/sdrclib */

//---------------------------------------------------------------------------
// char *src  -> Source to split
// char *starts[maxstarts] -> tokens starts are stored here (this points to src's memory)
// int  lengths[maxstarts] -> tokens lengthss are stored here
// char seps[maxstarts] -> separators found (first if contsep=1)
// maxstarts -> zizeof (starts)=zizeof (lengths)..
// char sep1, sep2, sep3  -> tokens separator
// char brksep  -> line break indicator
// char **brkat -->line break found at (no line break search performed if NULL)
// char  **lastparsed -->resume parsing from here
// contsep -> treat two-or-more separator as one separator OR add a   starts[current]=NULL for every two separators
// return -> num of tokens found

int str_split_mb (const char *src, size_t src_sz, const char* *starts,int *lengths, char *seps, int maxstarts, char sep1,char sep2, char sep3, char brksep, const char **brkat,const char **lastparsed, int contsep) {
	int nt=1; 
	int mode=0;
	const char *cc=src;
	const char *cc_end=src+src_sz;
	char c;
	
	if (!src) return 0;
	if (*src==0) return 0;
	if (!maxstarts) return 0;
	*starts=cc;
	*lengths=0;
	while (cc<cc_end) {
		c=*cc;
		if (mode==0 && (c==sep1 || c==sep2 || c==sep3 || c==brksep)) { //was text now sep
			mode=1; *seps=c;seps++;
			if (brkat && c==brksep) {
				*brkat=cc;
				mode=3;
				if (*lengths==0) { //separator begining
					*starts=NULL;
				}
				break;
			}
			if (*lengths==0) { //separator begining
				*starts=NULL;
				if (nt==maxstarts) break; //we can't enter mode 2 in separator
			}
		} else if (mode==1 && (c==sep1 || c==sep2 || c==sep3 || c==brksep)){ //was sep now sep
			if (!contsep) { //was sep now sep and not continueing separator: output an empty token
				starts++; nt++;  *seps=c;seps++;
				(*starts)=NULL;
				lengths++;
				(*lengths)=0;
				if (brkat && c==brksep) {
					*brkat=cc;
					mode=3;
					break;
				}
				if (nt==maxstarts) break; //we can't enter mode 2 in separator
			} else {//was sep now sep and  continueing separator:  check for break
				if (brkat && c==brksep) {
					*brkat=cc;
					mode=3;
					break;
				}
			}
		} else if (mode==1 && !(c==sep1 || c==sep2 || c==sep3 || c==brksep)) { //was sep now text
			starts++; nt++;
			(*starts)=cc;
			lengths++;
			(*lengths)=1;
			if (nt==maxstarts) mode=2; //we enter mode 2 so last elem contains all the remaining chars
			else mode=0;
		} else { // was text now text or serching for eos
			(*lengths)++;
		}
		cc++; //advance;
	}
	if (lastparsed) *lastparsed=cc;
	if (nt==maxstarts) return nt;
	if (mode==1) {
		starts++; nt++;
		(*starts)=NULL;
		lengths++;
		(*lengths)=0;
	};
	return nt;
}
