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
val=q[7]

#define BPLUCK(val,xin,reset,q) \
q[0]++;\
if(*q <= q[1]) ITEMP1 = *q;\
else { *q = ITEMP1 = 20; q[20] = reset; }\
q[7] = (1.-q[8]) * q[ITEMP1] + q[8] * q[7];\
FTEMP1 = q[9] * (q[2] * q[ITEMP1] + q[3] * q[6]);\
q[6] = q[ITEMP1];\
q[ITEMP1] = q[10] * FTEMP1 + q[4] - q[10] * q[5];\
q[5] = q[ITEMP1] = xin + q[ITEMP1];\
q[4] = FTEMP1;\
val=q[7]
