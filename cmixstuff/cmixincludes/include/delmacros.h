int ii,im1;
float xx,xfrac;

#define DELPUT(x,a,l)\
	*(a + (*l)++) = x;\
	if(*(l) >= *(l+1)) *l -= *(l+1);

#define DELGET(a,wait,l,result)\
	ii = *l - (int)(wait * SR +.5);\
	if(ii < 0)  {\
		ii += *(l+1);\
		if(ii < 0) result = 0; }\
	if(ii >= 0) result = (*(a+ii));

#define DLIGET(a,wait,l,result)\
	xx = wait * SR;\
	ii = (int)xx;\
	xfrac = xx - ii;\
	ii = *l - ii;\
	im1 = ii - 1;\
	if(ii <= 0) {\
		if(ii < 0) ii += *(l+1);\
		if(ii < 0) result = 0.;\
		if(im1 < 0) im1 += *(l+1); }\
	if(ii >= 0) result = (*(a+ii) + xfrac * (*(a+im1) - *(a+ii)));
