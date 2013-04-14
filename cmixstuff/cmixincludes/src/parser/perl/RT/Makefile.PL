use ExtUtils::MakeMaker;
# "perldoc ExtUtils::MakeMaker" for details of how to influence
# the contents of the Makefile that is written.
WriteMakefile(
    'NAME'         => 'RT',
#   'VERSION_FROM' => 'RT.pm',  # parses $VERSION in RT.pm; not reliable
    'VERSION'      => '0.02',  # be sure to change version in RT.pm.in also
    'PREREQ_PM'    => {},      # e.g., Module::Name => 1.1
    'LIBS'         => [''],    # e.g., '-lm'
    'DEFINE'       => '-g',    # e.g., '-DHAVE_SOMETHING'
    'INC'          => '',      # e.g., '-I/usr/include/other'
);
