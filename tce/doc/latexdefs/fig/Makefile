#
# Generate Encapsulated Postscript Pictures from
# - XFig Files		(fig2dev)
# - PNG Files		(bmeps)
# - DIA Files           (dia -n)
#
# support for PNG source file format disabled
# use 'bmeps' to translate PNG files to EPS

.PHONY: clean
.SUFFIXES: .png .fig .eps .dia

figs = $(wildcard *.fig)
pngs = $(wildcard *.png)
dias = $(wildcard *.dia)
pics = $(figs:.fig=.eps) $(pngs:.png=.eps) $(dias:.dia=.eps)

all: create_eps_dir $(pics) 

# translation into eps
.fig.eps:
	fig2dev -L eps $*.fig > $*.eps
	if test ! -h ../eps/$*.eps; then \
        ln -s ../fig/$*.eps ../eps/$*.eps; \
    fi

# translation of png into eps
.png.eps:
	bmeps $*.png $*.eps
	if test ! -h ../eps/$*.eps; then \
        ln -s ../fig/$*.eps ../eps/$*.eps; \
    fi

# translation of dia into eps (does not work with Swollen's version of dia)
.dia.eps:
	 dia -t eps -n $*.dia > /dev/null

clean: delete_eps_files delete_eps_dir	

delete_eps_files:
	$(foreach file, $(figs), \
	    rm -f $(subst .fig,.eps,$(file)); \
	    rm -f ../eps/$(subst .fig,.eps,$(file)); \
	)
	$(foreach file, $(pngs), \
	    rm -f $(subst .png,.eps,$(file)); \
	    rm -f ../eps/$(subst .png,.eps,$(file)); \
	)

create_eps_dir:
	if test ! -d ../eps ; \
	  then mkdir ../eps ; \
	fi

delete_eps_dir:
	if test -z "$(shell ls ../eps)" ; \
	  then rm -rf ../eps ; \
	fi
