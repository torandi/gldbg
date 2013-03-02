ACLOCAL_AMFLAGS = -I m4
AM_DEFAULT_SOURCE_EXT = .cpp


AM_CFLAGS = -std=gnu99 -Wall -Wvla -Wconversion -I${top_srcdir}/src -fvisibility=hidden ${GL_CFLAGS} ${GLEW_CFLAGS} ${SDL_CFLAGS}
AM_LDFLAGS = ${GLEW_LIBS} ${GL_LIBS} ${SDL_LIBS}

lib_LTLIBRARIES = libgldbg.la

libgldbg_la_SOURCES = \
	src/main.c \
	src/glinject.c \
	src/buffers.c \
	src/config.c \
	src/gldbg.c

.PHONY: convenience-link clean-convenience-link

convenience-link: $(lib_LTLIBRARIES)
	@for soname in `echo | $(EGREP) "^dlname=" $^ | $(SED) -e "s|^dlname='\(.*\)'|\1|"`; do  \
		echo "$$soname: creating convenience link from $(abs_builddir)/.libs to $(top_builddir)"; \
		rm -f $(top_builddir)/$$soname ; \
		test -e $(abs_builddir)/.libs/$$soname && \
		cd $(top_builddir) && \
		$(LN_S) $(abs_builddir)/.libs/$$soname $$soname || true;\
	done 

clean-convenience-link:
	@for soname in `echo | $(EGREP) "^dlname=" $(lib_LTLIBRARIES) | $(SED) -e "s|^dlname='\(.*\)'|\1|"`; do  \
		echo "$$soname: cleaning convenience links"; \
		test -L $(top_builddir)/$$soname && rm -f $(top_builddir)/$$soname || true; \
	done 

all-local:: convenience-link

clean-local:: clean-convenience-link
