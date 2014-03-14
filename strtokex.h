#ifndef strtokex_h_sdr
#define strtokex_h_sdr


#ifdef __cplusplus
extern "C" {
#endif

	
	typedef struct strtokex_ctx_t_t strtokex_ctx_t;
	struct strtokex_ctx_t_t {
		const char *currt;
		int currl;
		const char *nextt;
		const char *currd;
		int cleft;
	};
	void strtokex_init_ctx (const char *src,int srclen, strtokex_ctx_t *ctx);
	

	#define STRTOKEX_POSTLAST 0
	#define STRTOKEX_ISLAST 1
	#define STRTOKEX_NOTLAST 2
	
	int strtokex_next (strtokex_ctx_t *ctx, char d1, char d2, char d3, char skip1, char skip2);
	#define	STEX_DS(d,s) (d),(d),(d),(s),(s)
	#define	STEX_DDS(d1,d2,s) (d1),(d2),(d2),(s),(s)
	
#ifdef __cplusplus
//extern "C" {
}
#endif
#endif