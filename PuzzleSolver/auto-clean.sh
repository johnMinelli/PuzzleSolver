#! /bin/bash

# Clean out all of the cruft created when running autotools
if [ -e Makefile ]; then
    make clean
fi

rm -rf  \
Makefile.in \
depcomp \
INSTALL \
compile \
install-sh \
missing \
config.h.in* \
autom4te.cache \
configure \
aclocal.m4 \
AUTHORS \
NEWS \
README \
COPYING \
.deps \
Makefile \
config.status \
config.h \
config.log \
stamp-h1 \
