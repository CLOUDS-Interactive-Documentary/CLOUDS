#include <combs.h>
#include <math.h>

extern float FTEMP1,FTEMP2,FTEMP3,FTEMP4;
extern int ITEMP1,ITEMP2;

#define COMB(val,samp,a)\
if ( a[STARTM1] >= (int) a[0]) a[STARTM1] = START;\
{ float *PTEMP1 = a + (int)a[STARTM1];\
a[STARTM1] += 1;\
val = *PTEMP1;\
*PTEMP1 = *PTEMP1 * a[1] + samp; \
}

#define ALLPASS(val,samp,a)\
if ( a[STARTM1] >= (int) a[0]) a[STARTM1] = START;\
{ float *PTEMP1 = a + (int)a[STARTM1];\
a[STARTM1] += 1;\
val = *PTEMP1;\
*PTEMP1 = *PTEMP1 * a[1] + samp;\
val -= a[1] * *PTEMP1; \
}

#define OSCIL(val,amp,si,farray,len,phs) \
ITEMP1 = phs;\
phs += si;\
while(phs >= len)\
phs -= len;\
val= (*(farray+ITEMP1) * amp)

#define OSCILN(val,amp,si,farray,len,phs) \
ITEMP1 = phs;\
phs += si;\
while(phs >= len)\
phs -= len;\
while(phs < 0)\
phs += len;\
val= (*(farray+ITEMP1) * amp)

#define OSCILI(val,amp,si,farray,len,phs) \
ITEMP1 =  phs;\
ITEMP2 = (ITEMP1 + 1) % len;\
FTEMP1 = phs  - ITEMP1;\
phs += si;\
while(phs >= len)\
phs -= len;\
val= ((*(farray+ITEMP1)+(*(farray+ITEMP2)-*(farray+ITEMP1))*FTEMP1)*amp)

#define OSCILNI(val,amp,si,farray,len,phs) \
ITEMP1 =  phs;\
ITEMP2 = (ITEMP1 + 1) % len;\
FTEMP1 = phs  - ITEMP1;\
phs += si;\
while(phs >= len)\
phs -= len;\
while(phs < 0)\
phs += len;\
val= ((*(farray+ITEMP1)+(*(farray+ITEMP2)-*(farray+ITEMP1))*FTEMP1)*amp)

#define RESON(val,x,a)\
FTEMP1 = *a * x + *(a+1) * *(a+3) - *(a+2) * *(a+4);\
*(a+4) = *(a+3);\
val = *(a+3) = FTEMP1

#define BUZZ(val,amp,si,hn,f,phs)\
ITEMP1 = phs;\
ITEMP2 = (ITEMP1+1) % 1024;\
FTEMP1 = 2. * hn;\
FTEMP2 = FTEMP1 + 1.;\
FTEMP3 = (int)((phs - (float)ITEMP1) * FTEMP2)/FTEMP2;\
FTEMP4 = *(f+ITEMP1);\
FTEMP4 += (*(f+ITEMP2)-FTEMP4)*FTEMP3;\
if(!FTEMP4) FTEMP3 = amp;\
else {\
ITEMP2 = (long)(FTEMP2 * phs) % 1024;\
FTEMP3 = amp * (*(f+ITEMP2)/FTEMP4 - 1.)/FTEMP1;\
}\
phs += si;\
while(phs >= 1024.)\
phs -= 1024.;\
val = FTEMP3

#define ALLPOLE(x,jcount,npoles,past,c)\
for(ITEMP1=jcount,ITEMP2=0; ITEMP2<npoles;ITEMP2++,ITEMP1++)\
x += (*(c+ITEMP2) * *(past+ITEMP1));\
*(past+jcount) = *(past+jcount+npoles) = x;\
jcount=(jcount+1) % npoles


#define HPLUCK(val,xin,q) \
q[0]++;\
if(*q <= q[1]) ITEMP1 = *q;\
else *q = ITEMP1 = 20;\
q[7] = (1.-q[8]) * q[ITEMP1] + q[8] * q[7];\
FTEMP1 = q[9] * (q[2] * q[ITEMP1] + q[3] * q[6]);\
q[6] = q[ITEMP1];\
q[ITEMP1] = q[10] * FTEMP1 + q[4] - q[10] * q[5];\
q[5] = q[ITEMP1] = xin + q[ITEMP1];\
q[4] = FTEMP1;\
val=q[7];

#define HCOMB(hc,xin,rvt,a)\
if (rvt != a[2]) { if (rvt) a[3] = pow(.001,(a[1]/rvt));\
else  a[3] = 0.;\
a[2] = rvt; }\
if (a[4] >= a[0]) a[4] = 10.;\
else  a[4]++;\
ITEMP1 = a[4];\
hc = a[ITEMP1];\
FTEMP1 = a[3] * a[ITEMP1];\
a[7] = a[8] * FTEMP1 + a[6] - a[8] * a[7];\
a[ITEMP1] = xin + a[7];\
a[6] = FTEMP1;

#define EVP(val,nsample,f1,f2,q)\
FTEMP1 = (float)(nsample)/ q[0];\
if( FTEMP1 > q[1]) {\
ITEMP1 = (1. - ((FTEMP1 - q[1])/ q[4])) * q[3];\
val = f2[ITEMP1];\
}\
else \
if(FTEMP1 >= q[2]) val=1;\
else	{\
ITEMP1 = FTEMP1 * q[3]/ q[2];\
val = f1[ITEMP1];\
}
