all: mag plot	

mag: mag.cxx
	g++ -c -O3 mag.cxx `root-config --cflags` -I$(GARFIELD_HOME)/Include
	g++ -O3 -Wall -o mag mag.o \
	$(GARFIELD_HOME)/Library/libGarfield.a \
	-lgfortran `root-config --glibs`	
	rm mag.o

plot: plot.cxx
	g++ -c -O3 plot.cxx `root-config --cflags` -I$(GARFIELD_HOME)/Include
	g++ -O3 -Wall -o plot plot.o \
	$(GARFIELD_HOME)/Library/libGarfield.a \
	-lgfortran `root-config --glibs`	
	rm plot.o

clean:
	rm -f *.o
	rm -f mag plot
