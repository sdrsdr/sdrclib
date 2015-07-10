
#include <stdlib.h>
#include "str_split.h"

/* Part of sdrclib from https://github.com/sdrsdr/sdrclib */

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

int msplit_str (char *src, char* *starts,int *lengths, char *seps, int maxstarts, char sep1,char sep2, char sep3, char brksep, char **brkat,char **lastparsed, int contsep) {
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

char *strtrim(char *start, int *len, char trim1, char trim2, char trim3){
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
