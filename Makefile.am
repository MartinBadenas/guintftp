AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS = guintftp
guintftp_SOURCES = src/daemon/main.c src/daemon/tftp_configuration.c \
					src/daemon/tftp_configuration.h src/daemon/tftp_io.h src/daemon/tftp_io.c \
					src/daemon/tftp_management.c src/daemon/tftp_management.h src/daemon/tftp_mode.c \
					src/daemon/tftp_mode.h src/daemon/tftp_net.c src/daemon/tftp_net.h \
					src/daemon/tftp_packet.c src/daemon/tftp_packet.h

dist_noinst_SCRIPTS = autogen.sh