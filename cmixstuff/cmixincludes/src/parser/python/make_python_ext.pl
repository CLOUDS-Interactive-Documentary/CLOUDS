# This script writes the extension .c file to stdout.  It reads a list of
# RTcmix function names from a file, and uses them in the .c file, so that
# Python will recognize them in RTcmix user scripts.     -JGG, 12-Feb-01

# names of module and verbatim src file passed in as args
$module = $ARGV[0];
$modulesrc = $ARGV[1];
$funclist = "./funclist";

# Open file containing RTcmix function names, one per line.  Save these so
# that we can use them below to build the individual dispatcher functions.

open FUNCLIST, "< $funclist" || die "Can't open $funclist ($!)";

while (<FUNCLIST>) {
   chop;
   push(@functions, $_);
}
close FUNCLIST;

open MODULESRC, "< $modulesrc" || die "Can't open $modulesrc ($!)";

# Print the .cpp file to stdout.

while (<MODULESRC>) {
   print $_;
}
close MODULESRC;


print qq~
extern "C" {

~;

# Append individual dispatcher functions.

print "// Dispatcher functions for RTcmix calls. Note: <self> not used.\n";
foreach $func (@functions) {
print qq~
static PyObject *_$func(PyObject *self, PyObject *args)
{
   return _call_dispatch(args, \"$func\");
}
~;
}


# Print _rtcmix_methods[], initialized with names of functions defined above.
print "\n\n// List of functions defined in the module\n";

print "\nstatic PyMethodDef _rtcmix_methods[] = {\n";
foreach $func (@functions) {
   print "   {\"$func\", _$func, METH_VARARGS},\n";
}
print "   {NULL, NULL}        // sentinel\n";
print "};\n\n";


print qq~
// Initialization function for the module (*must* be called init$module)
DL_EXPORT(void) init$module()
{
   // Initialize the type of the new type object here; doing it here
   // is required for portability to Windows without requiring C++.
   OpaqueType.ob_type = &PyType_Type;

   // Create the module and add the functions
   PyObject *m = Py_InitModule("$module", _rtcmix_methods); // aborts internally

   // Add some symbolic constants to the module
   PyObject *d = PyModule_GetDict(m);
   ErrorObject = PyErr_NewException((char *) "$module.error", NULL, NULL);
   PyDict_SetItemString(d, (char *) "error", ErrorObject);
}

} // extern "C"

~;

