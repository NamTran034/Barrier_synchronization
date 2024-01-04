OMPFLAGS = -fopenmp 		# OMP compiler flags 
OMPLIBS = -lgomp -lm		# OMP runtime library and math library

CC = gcc					# C compiler 
CPPFLAGS = -g $(OMPFLAGS)	# C preprocessor flags, includes debugging flag and OMP flags
LDFLAGS = -g $(OMPFLAGS) 	# Linker flags, including debugging flag and OMP flags
LDLIBS = $(OMPLIBS)			# Linker libraries, include the OMP libraries

# The default target. It depends on three other targets 
all: tournament dissemination centralized

tournament: tournament.o
	        $(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

dissemination: dissemination.o
	        $(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

centralized: centralized.o
	        $(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# A target to remove the object files (*.o) and the executables. 
# This is useful for cleaning up the project directory.
clean: rm -f *.o tournament dissemination centralized
