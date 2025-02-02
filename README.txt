		RCSid "$Id: README.txt,v 1.9 2023/11/15 18:02:52 greg Exp $"

Hello and welcome to the Radiance synthetic imaging system.  This is
the twenty-sixth release, Version 6.0, and it includes all source files
for local compilation with a standard C compiler.  To find out what's
new in this release, check the text file "doc/notes/ReleaseNotes".

Radiance is open source software copyrighted and distributed by
Lawrence Berkeley National Laboratory in California.  By downloading
and installing this software, you are implicitly agreeing to the
copyright appended to this README file, and the license agreement (License.txt)
in the same directory.  Please read both carefully before proceeding.

Please read this entire file before sending e-mail asking
how to install this software or what to do with it.  Some
frequently asked questions are answered here and also on our
website at:

	https://www.radiance-online.org/

The CVS source tree is available, including post-release bug-fixes and
mailing lists access at:

	https://www.radiance-online.org/download-install/CVS%20source%20code

The website below has some precompiled Radiance binaries
and add-ons for common Unix platforms:

	https://www.radiance-online.org/download-install/installation-information

We have attempted to make it easy for you (and for us) to install
the software on differently configured systems using a global make
script.  To install the software, just type:

	./makeall install

You can clean up the .o files and so forth with:

	./makeall clean

Or, if you are confident you can do both at the same time with:

	./makeall install clean

You can give other make options at the end of the command as well.  For
example, the -n option will tell you what makeall is going to do without
actually doing it.

If you downloaded the binaries, so compilation is not necessary,
run the following command to install the library files only:

	./makeall library

The makeall script may ask you questions about your system and where you
want to install the executables and library files.  The pathnames you
give should be relative to root for the programs to work properly.  You
may also use the tilde ('~') character to give paths starting with
someone's home directory.

If you do NOT have X11 support, please read the note "noX11.help" in
this directory.  It explains what to change to make things work.

Although it is set automatically by makeall, individuals may want to
set the RAYPATH environment variable manually.  This variable tells
Radiance where to look for auxiliary files, and usually includes the
current directory as well as the system library (ray/lib in this
distribution).  As you develop auxiliary files yourself, it is often
useful to add in your own library directory before the system directory.
An example setting such as this would go in a user's .login file:

	setenv RAYPATH .:${HOME}/mylib:/usr/local/lib/ray

After installing the software, you may want to start by scanning the
troff input documentation contained in "doc/ray.1", which is also formatted
for web browsers in "doc/ray.html".
A PDF version of this manual may be found in
"doc/pdf/ray.pdf", along with an older tutorial by Cindy Larson,
"doc/pdf/raduser1_6.pdf".

To check that everything is peforming correctly, you can run:

	./makeall test

For complete documentation on the system as of release 3.1, refer to
"Rendering with Radiance" by Larson and Shakespeare, which used to be
available from Morgan Kauffman Publishing.  Recently, the authors have
taken over printing of the book, which is now available from booksurge.com.
You may find it linked to the Radiance website page:

	http://radsite.lbl.gov/radiance/book/

The first chapter of this book, which includes a basic tutorial and
serves as a good introduction for new users, may be found in
"doc/pdf/RwR01.pdf".

Important additions since version 3.1 include the holodeck programs (rholo,
etc., built in src/hd), glrad, ranimove, the new "mesh" primitive,
and rtcontrib.  Again, check doc/notes/ReleaseNotes for details.

Individual manual pages may be found in the subdirectory "doc/man".
You may want to copy these to the system manual directory, or add this
directory to your MANPATH environment variable in your shell
initialization file.  A good starting place is to print the text file
ray/doc/man/whatis, which gives a one line description of each program in
the Radiance package.  To print out the manual pages, use the "-man"
macro package.

The most important program to learn about if you are a new user of Radiance
(or have not used this program before) is "rad".  It controls Radiance
lighting parameters, and automates much of the rendering process.
To get started, change directory to obj/misc and try running:

	% rad -o x11 daf.rif

This works if you are running X11.  If you are not, you can still generate
an image with:

	% rad daf.rif &

When the picture is done, you can convert it to another format for display
with any of the ra_* programs listed in the doc/whatis database.

There is a user interface built on top of this using Tcl/Tk, called trad.
If you do not have Tcl/Tk installed on your system, you must do so prior
to building this distribution if you want trad to install.  (Try a google
search on "Tcl/Tk" or go to "http://www.scriptics.com/software/tcltk/".)

+++++++++++++++++ COPYRIGHT NOTICE +++++++++++++++++

Radiance v5.4 Copyright (c) 1990 to 2022, The Regents of the University of 
California, through Lawrence Berkeley National Laboratory (subject to receipt 
of any required approvals from the U.S. Dept. of Energy).  All rights reserved.

If you have questions about your rights to use or distribute this software,
please contact Berkeley Lab's Intellectual Property Office at
IPO@lbl.gov.

NOTICE.  This Software was developed under funding from the U.S. Department
of Energy and the U.S. Government consequently retains certain rights.  As
such, the U.S. Government has been granted for itself and others acting on
its behalf a paid-up, nonexclusive, irrevocable, worldwide license in the
Software to reproduce, distribute copies to the public, prepare derivative 
works, and perform publicly and display publicly, and to permit others to do so.

