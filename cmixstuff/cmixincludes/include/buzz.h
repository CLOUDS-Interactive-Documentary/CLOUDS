float FTEMP1,FTEMP2,FTEMP3,FTEMP4;
int   ITEMP1,ITEMP2;
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
