#!/bin/csh -fe
# RCSid: $Id: tradinstall.csh,v 2.11 2020/08/27 17:46:52 greg Exp $
# Install correct version of trad for wish or wish4.0
#
set instdir = $1
set libdir = $2

# Need to execute in containing directory:
if ($0:h != $0) cd $0:h

set TLIBFILES = ( *[a-z].tcl *.hlp trad.icon tclIndex )

set TDIFFS = (`ls | sed -n 's/3\.6\.tcl$//p'`)

set WISHCOMS = ( wish4.{3,2,1,0} wish8.{6,5,4,3,2,1,0} wish )

foreach w ( $WISHCOMS )
	foreach d ($path)
		if (-x $d/$w) then
			set wishcom = $d/$w
			break
		endif
	end
	if ( $?wishcom ) break
end
if (! $?wishcom) then
	echo "Cannot find wish executable in current path -- trad not installed."
	exit 1
endif
if ("`file $wishcom`" =~ "*shell script*") then
	set wishcom="/bin/sh"
endif

echo "Installing trad using $wishcom"

sed -e "1s|/usr/bin/wish|$wishcom|" \
	-e "s|^set radlib .*|set radlib $libdir|" trad.wsh > $instdir/trad
chmod 755 $instdir/trad
if (! -d $libdir) then
	mkdir $libdir
endif
(cd $libdir ; rm -f $TLIBFILES)
cp $TLIBFILES $libdir
