# Filter out architectures other than the one we're building.
#!/bin/sh

if [ $# -lt 1 ]
then
   echo  Usage: `basename $0` arch
   exit 1
fi

arch=$1

if [ -e "makefile.tmp" ]; then
	rm -f "makefile.tmp"
fi
if [ $arch = "x86_64" ]; then
	cat "Makefile" | sed 's/-arch ppc//' | sed 's/-arch i386//' > "makefile.tmp"
elif [ $arch = "i386" ]; then
	cat "Makefile" | sed 's/-arch ppc//' | sed 's/-arch x86_64//' > "makefile.tmp"
else # arch == ppc
	cat "Makefile" | sed 's/-arch i386//' | sed 's/-arch x86_64//' > "makefile.tmp"
fi

mv -f "makefile.tmp" "Makefile"
