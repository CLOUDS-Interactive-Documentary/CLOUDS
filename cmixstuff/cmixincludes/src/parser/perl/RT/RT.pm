package RT;

# JGG: This will have to be relaxed at some point...
require 5.005_62;
use strict;

use warnings;

require Exporter;
require DynaLoader;

our @ISA = qw(Exporter DynaLoader);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use RT ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(
	
) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(
AUTOLOAD
srand
rand
reset
);

our $VERSION = '0.02';

bootstrap RT $VERSION;

# Preloaded methods go here.

# This is what Perl calls any time it runs across an unknown subroutine

sub AUTOLOAD {
	my $cmd = $RT::AUTOLOAD;
	$cmd =~ s/.*:://;	# strip the main:: prefix off
	return RT::handle_minc_function($cmd, @_);
}

# These are subroutines we want to have RTcmix versions of

sub srand {
	return RT::handle_minc_function("srand", @_);
}

sub rand {
	return RT::handle_minc_function("rand", @_);
}

sub reset {
	return RT::handle_minc_function("reset", @_);
}

sub open {
	return RT::handle_minc_function("open", @_);
}

sub print {
	return RT::handle_minc_function("print", @_);
}

sub splice {
	return RT::handle_minc_function("splice", @_);
}

1;
__END__
# Below is stub documentation for your module. You better edit it!

=head1 NAME

RT - Perl extension that provides RTcmix functions to a Perl interpreter
embedded in RTcmix.

=head1 SYNOPSIS

use RT;

Then call any RTcmix function from within a Perl script.  There's no need 
to prefix "RT::" to them.

=head1 DESCRIPTION

This extension creates glue code that interfaces Perl with the RTcmix
dispatcher.  Perl passes all unknown subroutine names (i.e., functions
that you might use in an RTcmix script) to the autoloader, which calls
the glue subroutine, which in turn calls the RTcmix function parse_dispatch
with the function name string and your pfields.

This only works when RTcmix has been compiled with the option to embed
the Perl interpreter in RTcmix.

=head2 EXPORT

In addition to AUTOLOAD, a few RTcmix function names, like srand, rand
and reset, are exported, so that the Perl script will use the RTcmix
versions of these by default, instead of the Perl ones.  (You can still
get the Perl ones by using the CORE:: prefix.)

=head1 AUTHOR

John Gibson, <johngibson@virginia.edu>
Rewritten and adapted by Douglas Scott.

=head1 SEE ALSO

perl(1), perlxs(1), perlxstut(1), perlguts(1), perlembed(1).

=cut
