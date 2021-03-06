#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include <immintrin.h>
#include "lte_phy.h"

#define MIC_DEV 1
#define LEN8 8
#define LEN16 16

typedef __attribute__((aligned(64))) union zmmi {
		__m512i reg;
		int elems[LEN16];
} zmmi_t;

typedef __attribute__((aligned(64))) union zmmf {
    __m512 reg;
    float elems[LEN16];
} zmmf_t;


void GenScrambInt(int *pScrambInt, int n)
{
	int i;
	int N_c = 1600;
	
	int n_init[31] = { 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0};

	/////////////////////Generate ScrambSeq///////////////////////
	int px1[N_SCRAMB_IN_MAX + 1600];
	int px2[N_SCRAMB_IN_MAX + 1600];

	for (i = 0; i < 31; i++)
	{
		px1[i] = 0;
		px2[i] = n_init[i];
	}
	px1[0] = 1;
	
	for (i = 0; i < n + N_c - 31; i++)
	{
		px1[i + 31] =(px1[i + 3] + px1[i]) % 2;
		px2[i + 31] = (px2[i + 3] + px2[i + 2] + px2[i + 1] + px2[i]) % 2;
	}
	for (i = 0; i < n; i++)
	{
		pScrambInt[i] = 3; // What is this? Any use?
		pScrambInt[i] = (px1[i + N_c] + px2[i + N_c]) % 2;
	}
	/////////////////////END Generate ScrambSeq///////////////////////
}

void Scrambling(LTE_PHY_PARAMS *lte_phy_params, int *pInpSeq, int *pOutSeq)
{
	int n_inp;
	int *scramb_seq_int;
	int offset,n_threads=236;
	int i,j;
	//printf("%d ",n_inp);
	n_inp = lte_phy_params->scramb_in_buf_sz;
	scramb_seq_int=lte_phy_params->scramb_seq_int;
	//GenScrambInt(scramb_seq_int, n_inp);

	////////////////////////Scrambling////////////////////////////

//#pragma offload_transfer target(mic:MIC_DEV) \
	in(pOutSeq:length(n_inp) ALLOC RETAIN) \
	in(pInpSeq:length(n_inp) ALLOC RETAIN) \
	in(scramb_seq_int:length(n_inp) ALLOC RETAIN)
	//omp_set_num_threads(236);
	//n_threads = omp_get_num_threads();
//	offset = n_inp / n_threads + 1;
//#pragma omp parallel default(shared) num_threads(n_threads)	
	/*for (i = 0; i < n_threads; i++)
	{
		int j_end;
		if(i!=n_threads-1) j_end=offset*(i+1);
		else j_end=n_inp;
		for(j=offset*i;j<j_end;j++)
		pOutSeq[j] = (pInpSeq[j] + scramb_seq_int[j]) % 2;
	}*/
//    printf("left\n");
    for (i = 0; i < n_inp; i+=16)
    {
        zmmi_t tmp_f,tmp_f2,tmp_result;
        tmp_f.elems[0:LEN16] = pInpSeq[i:16];
        tmp_f2.elems[0:LEN16] = scramb_seq_int[i:16];
        tmp_result.reg = _mm512_add_epi32(tmp_f.reg, tmp_f2.reg);
        pOutSeq[i:16] = tmp_result.elems[0:LEN16] % 2;
    }
    
    i-=16;
    for(;i<n_inp;i++)
    {
        pOutSeq[i] = (pInpSeq[i] + scramb_seq_int[i]) % 2;
    }
/*	for (i = 0; i < n_inp; i++)
	{
		pOutSeq[i] = (pInpSeq[i] + scramb_seq_int[i]) % 2;
	}*/
	////////////////////////END Scrambling////////////////////////////
}

void Descrambling(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pOutSeq)
{
	int n_inp;
//	float scramb_seq_float[N_SCRAMB_IN_MAX];
	int *scramb_seq_int;
	int i,j;
	n_inp = lte_phy_params->scramb_in_buf_sz;
	scramb_seq_int=lte_phy_params->scramb_seq_int;
	for (i = 0; i < n_inp-16; i+=16)
	{
		zmmf_t tmp_f,tmp_f2,tmp_result;
		zmmi_t tmp_i;
		tmp_f.elems[0:LEN16] = (float)scramb_seq_int[i:LEN16] * -2.0 + 1.0 ;
		tmp_f2.elems[0:LEN16] = pInpSeq[i:16];
		tmp_result.reg = _mm512_mul_ps(tmp_f.reg, tmp_f2.reg);
		pOutSeq[i:LEN16] = tmp_result.elems[0:16];
	}

	for(;i<n_inp;i++)
	{
		pOutSeq[i] = (pInpSeq[i] * (scramb_seq_int[i] * (-2.0) + 1.0));
	}
	//double ttime = ddtime()-tstart;
	//printf("%f s\n",ttime);
}
//#endif
