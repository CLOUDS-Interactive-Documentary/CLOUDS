#define byte_reverse8(data)                                    \
    { char c, *t; t = (char *) data;                           \
    c = t[0]; t[0] = t[7]; t[7] = c;                           \
    c = t[1]; t[1] = t[6]; t[6] = c;                           \
    c = t[2]; t[2] = t[5]; t[5] = c;                           \
    c = t[3]; t[3] = t[4]; t[4] = c; }

#define byte_reverse4(data)                                    \
    { char c, *t; t = (char *) data;                           \
    c = t[0]; t[0] = t[3]; t[3] = c;                           \
    c = t[1]; t[1] = t[2]; t[2] = c; }

#define byte_reverse2(data)                                    \
    { char c, *t; t = (char *) data;                           \
    c = t[0]; t[0] = t[1]; t[1] = c; }

#define reverse_int4(data)                                     \
    (((*(unsigned long int *) (data) & 0x000000ffU) << 24) |   \
     ((*(unsigned long int *) (data) & 0x0000ff00U) <<  8) |   \
     ((*(unsigned long int *) (data) & 0x00ff0000U) >>  8) |   \
     ((*(unsigned long int *) (data) & 0xff000000U) >> 24));

#define reverse_int2(data)                                     \
    (((*(unsigned short int *) (data) & 0x00ff) << 8) |        \
     ((*(unsigned short int *) (data) & 0xff00) >> 8));







