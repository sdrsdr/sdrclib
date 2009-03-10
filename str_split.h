#ifndef str_split_h_SDR_321234msddaa_included
#define str_split_h_SDR_321234msddaa_included


 
//---------------------------------------------------------------------------
// char *src  -> Source to split
// char *starts[maxstarts] -> tokens starts are stored here (this points to src's memory)
// int  lengths[maxstarts] -> tokens lengthss are stored here
// maxstarts -> zizeof (starts)=zizeof (lengths)
// sep -> tokens separator
// contsep -> treat two-or-more separator as one separator OR add a   starts[current]=NULL for every two separators
// return -> num of tokens found

#ifdef  __cplusplus
extern "C" {
#endif

int split_str (char *src, char* *starts,int *lengths, int maxstarts, char sep, int contsep);

#ifdef  __cplusplus
}
#endif

#endif
