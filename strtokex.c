#include <string.h>
#include "strtokex.h"

/*
typedef struct strtokex_ctx_t_t strtokex_ctx_t;
struct strtokex_ctx_t_t {
	 char *currt;
	 int currl;
	 char *nextt;
	 char *currd;
	 int cleft;
};
 
*/
void strtokex_init_ctx (const char *src,int srclen, strtokex_ctx_t *ctx){
	ctx->currt=NULL;
	ctx->currl=0;
	ctx->nextt=src;
	ctx->currd=NULL;
	ctx->cleft=srclen;
}

int strtokex_next (strtokex_ctx_t *ctx, char d1, char d2, char d3, char skip1, char skip2){
	if (ctx->cleft==0) {
		ctx->currt=NULL;
		ctx->currl=0;
		return STRTOKEX_POSTLAST;
	}

	ctx->currt=ctx->nextt;
	ctx->currl=0;
	ctx->currd=NULL;
	
	int finalskip=0;
	
	while (ctx->cleft) {
		char cc=*(ctx->nextt);
		if (finalskip){
			if (cc==skip1 || cc==skip2) {
				ctx->nextt++; ctx->cleft--;
				continue;
			} 
			//end final skip
			break;
		}
		if (cc==d1 || cc==d2 || cc==d3){
			ctx->currd=ctx->nextt;
			finalskip=1;
			
			ctx->nextt++; ctx->cleft--;
			
			
			
			continue;
			
		}
		
		if (cc==skip1 || cc==skip2) {
			ctx->nextt++; ctx->cleft--;
			continue;
		}
		ctx->nextt++; ctx->cleft--;
		ctx->currl=(ctx->nextt)-(ctx->currt);
	}
	if (ctx->currl==0) {
		ctx->currt=NULL;
		ctx->currd=NULL;
		return STRTOKEX_POSTLAST;
	}
	if (ctx->cleft) return STRTOKEX_NOTLAST;
	ctx->currd=NULL;
	ctx->nextt=NULL;
	return STRTOKEX_ISLAST;
}