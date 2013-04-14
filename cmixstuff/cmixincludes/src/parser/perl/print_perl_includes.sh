#!/bin/sh

if [ $# -lt 2 ]
then
   echo  Usage: `basename $0` perl arch
   exit 1
fi

perl=$1
arch=$2

if [ $arch = "x86_64" ]; then
	echo `$perl -MExtUtils::Embed -e ccopts | sed 's/-arch ppc//' | sed 's/-arch i386//'`
elif [ $arch = "i386" ]; then
	echo `$perl -MExtUtils::Embed -e ccopts | sed 's/-arch ppc//' | sed 's/-arch x86_64//'`
else # arch == ppc
	echo `$perl -MExtUtils::Embed -e ccopts | sed 's/-arch i386//' | sed 's/-arch x86_64//'`
fi

