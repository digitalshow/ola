# LIBRARIES
if HAVE_LIBUSBDMX
  lib_LTLIBRARIES += plugins/enlightenment/libolaenlightenment.la
  plugins_enlightenment_libolaenlightenment_la_SOURCES = \
                 plugins/enlightenment/EnlightenmentPlugin.h \
                 plugins/enlightenment/EnlightenmentDevice.h \
                 plugins/enlightenment/EnlightenmentPort.h \
                 plugins/enlightenment/EnlightenmentPlugin.cpp \
                 plugins/enlightenment/EnlightenmentDevice.cpp \
                 plugins/enlightenment/EnlightenmentPort.cpp
  plugins_enlightenment_libolaenlightenment_la_CXXFLAGS = $(COMMON_CXXFLAGS) $(libusbdmx_CFLAGS)
  plugins_enlightenment_libolaenlightenment_la_LIBADD = $(libusbdmx_LIBS)
endif
