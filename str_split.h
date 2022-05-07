#ifndef str_split_h_SDR_321234msddaa_included
#define str_split_h_SDR_321234msddaa_included

/* Part of sdrclib from https://github.com/sdrsdr/sdrclib */
 
//---------------------------------------------------------------------------
// char *src  -> Source to split
// char *starts[maxstarts] -> tokens starts are stored here (this points to src's memory)
// int  lengths[maxstarts] -> tokens lengthss are stored here
// maxstarts -> sizeof (starts)=sizeof (lengths)
// sep -> tokens separator
// contsep -> treat two-or-more separator as one separator OR add a   starts[current]=NULL for every two separators
// return -> num of tokens found

#ifdef  __cplusplus
extern "C" {
#endif

int str_split (const char *src, const char* *starts,int *lengths, int maxstarts, char sep, int contsep);

//same as sat_split with multiple separator and a break separator which needs brkat set to non NULL to triger and store break position, lastparsed holds ponter to last parsed char
int str_split_m (const char *src, const char* *starts,int *lengths, char *seps, int maxstarts, char sep1,char sep2, char sep3, char brksep, const char **brkat,const char **lastparsed, int contsep);

//same as str_split_m but binary safe(\0 is considered data): you need to supply src size in src_sz
int str_split_mb (const char *src, size_t src_sz, const char* *starts,int *lengths, char *seps, int maxstarts, char sep1,char sep2, char sep3, char brksep, const char **brkat,const char **lastparsed, int contsep);


#ifdef  __cplusplus
}
#endif

#endif
