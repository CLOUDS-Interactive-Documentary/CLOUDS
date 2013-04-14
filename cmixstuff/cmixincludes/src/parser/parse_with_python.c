/* RTcmix  - Copyright (C) 2001 The RTcmix Development Team
   See ``AUTHORS'' for a list of contributors. See ``LICENSE'' for
   the license to this software and for a DISCLAIMER OF ALL WARRANTIES.
*/

/* Functions for managing embedded Python parser.    -JGG, 12-Feb-01 */

#include <assert.h>
#include <Python.h>
#include "rtcmix_parse.h"
#include <Option.h>

#ifdef PYEXT_INIT
extern void initrtcmix(void);		/* defined in rtcmixmodule.cpp */
#endif

/* ========================================================================== */
/* These functions called when running interactively, from the PYRTcmix object.
*/

/* ----------------------------------------------------- python_parse_buf --- */
int
python_parse_buf(char *inBuf)
{
	static int python_inited = 0;
	int status;

	if (!python_inited) {
		/* Init Python interpreter.  If this fails, it won't return. */
		Py_Initialize();
		python_inited = 1;
	}

	status = PyRun_SimpleStringFlags(inBuf, NULL);

	return status;
}


/* ------------------------------------------------------ get_python_fval --- */
double
get_python_fval(char *val)
{
	// FIXME
	return 0.0;
}

/* ------------------------------------------------------ python_finalize --- */
void
python_finalize()
{
	Py_Finalize();				/* any errors ignored internally */
}


/* ========================================================================== */
/* These functions called when running non-interactively, from scripts. */

static FILE *_script = NULL;
static char *_script_name = NULL;

/* ---------------------------------------------------------- parse_score --- */
int
parse_score(int argc, char *argv[], char **env)
{
	int	status, xargc;
	char	*xargv[MAXARGS + 2];

	assert(argc <= MAXARGS);

	Py_SetProgramName(argv[0]);

	/* Init Python interpreter.  If this fails, it won't return. */
	Py_Initialize();

	/* Define sys.argv in Python. */
	PySys_SetArgv(argc, argv);

	/* If we're linking statically to extension module, init it */
#ifdef PYEXT_INIT
	initrtcmix();
#endif

	if (_script == NULL)
		_script = stdin;
	/* Otherwise, <_script> will have been set by use_script_file. */

	/* Run the Python interpreter. */
	PyRun_AnyFile(_script, _script_name);

	/* Kill interpreter, so that it won't trap cntl-C while insts play.
	   Actually, it turns out that this doesn't help, at least for 
	   Python 2.x, so we have to reinstall our SIGINT handler in main().
	*/
	Py_Finalize();				/* any errors ignored internally */

	return 0;
}


/* ------------------------------------------------------ use_script_file --- */
/* Parse file <fname> instead of stdin. */
void
use_script_file(char *fname)
{
	_script = fopen(fname, "r");
	if (_script == NULL) {
		fprintf(stderr, "Can't open %s\n", fname);
		return;
	}
	_script_name = fname;
	if (get_bool_option(kOptionPrint))
		printf("Using file %s\n", fname);
}


/* ------------------------------------------------------- destroy_parser --- */
void
destroy_parser()
{
	/* nothing to do (see Py_Finalize() above) */
}

