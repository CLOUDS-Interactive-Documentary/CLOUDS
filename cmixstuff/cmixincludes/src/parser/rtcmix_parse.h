/* rtcmix_parse.h */
#ifdef __cplusplus
extern "C" {
#endif

#define MAXARGS   32

#ifdef MAXMSP
int parse_score(char *thebuf, int buflen);
#else
int parse_score(int argc, char *argv[], char **env);
#endif

int perl_parse_buf (char *inBuf);
void set_perl_var(char *,double);
double get_perl_var(char*);
double get_perl_fval(char*);
int python_parse_buf (char *inBuf);
double get_python_fval(char *);
void use_script_file(char *fname);
void destroy_parser(void);

#ifdef __cplusplus
} /* extern "C" */
#endif
