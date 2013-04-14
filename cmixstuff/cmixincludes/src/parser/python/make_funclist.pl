# Scan the RTcmix directory tree for the names of all possible RTcmix
# Minc functions.  These appear in the UG_INTRO and RT_INTRO macros in
# .c and .C files.  Filter out duplicates and any items appearing in an
# exceptions file.  Write the result to an output file.  Currently, there
# is no way to add other names to this file (such as would be needed for
# user functions).   -JGG, 28-Jul-00

$cmixdir = $ARGV[0];             # location of rtcmix passed in as arg
$output = "./funclist";
$exceptions = "./funcexcept";
$grep = "grep";
$find = "find";
if ($#ARGV == 1) {
   $flags = $ARGV[1];   # flags to pass to find command
}
else {
   $flags = "";
}

# Grep through RTcmix src tree for functions in UG_INTRO and RT_INTRO macros.
$ug_intro_c = $cmixdir . "/src/rtcmix/ug_intro.c";
$cmd = "$grep UG_INTRO $ug_intro_c";
@list1 = `$cmd`;
push(@list, @list1);
$ug_intro_c = $cmixdir . "/src/rtcmix/rt_ug_intro.cpp";
$cmd = "$grep UG_INTRO $ug_intro_c";
@list1 = `$cmd`;
push(@list, @list1);
$instdir = $cmixdir . "/insts";
$cmd = "$find $instdir $flags -name \"*.c\" | xargs $grep UG_INTRO";
@list1 = `$cmd`;
push(@list, @list1);
$cmd = "$find $instdir $flags -name \"*.cpp\" | xargs $grep UG_INTRO";
@list1 = `$cmd`;
push(@list, @list1);
$cmd = "$find $instdir $flags -name \"*.cpp\" | xargs $grep RT_INTRO";
@list1 = `$cmd`;
push(@list, @list1);

# Strip out everything except what's inside the double quotes.
# NOTE: This requires that calls to the *INTRO macros end with a semicolon.
foreach $line (@list) {
   $line =~ s/.*\"(.*)\".*;.*/$1/g;
}

# Remove duplicates and sort (from perlfaq4).
undef %tmp;
@tmp{@list} = ();
@list = sort keys %tmp;

# Read exceptions file.
open EXCEPTIONS, "< $exceptions" || die "Can't open $exceptions ($!)";

# Remove from our function list any names matching those in exceptions file.
while (<EXCEPTIONS>) {
   $i = 0;
   foreach $func (@list) {
      if ($func eq $_) {
         splice @list, $i, 1;      # remove element; shift down ones following
      }
      else {
         $i++;
      }
   }
}
close EXCEPTIONS;

# Write to output file.
open OUTPUT, "> $output" || die "Can't open $output ($!)";
print OUTPUT @list;
close OUTPUT;

