#! /bin/bash

# After an update to configure.ac and/or Makefile.am, run this script.
touch NEWS README AUTHORS
autoreconf -vif
