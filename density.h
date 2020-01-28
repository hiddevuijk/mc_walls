#ifndef GUARD_DENSITY_H
#define GUARD_DENSITY_H


#include <vector>
#include <math.h>
#include <fstream>
#include <string>

#include "xyz.h"
#include "particle.h"


class Density {

public:
	// constructor
	Density(double LL, unsigned int Nbinn, double bss);

	// sample
	void sample( const std::vector<Particle> &particles , unsigned int N);

	// normalize
	void normalize();

	// get current number of samples taken
	int get_Nsamp() { return Nsamp;}

	// write to out stream
	void write(std::ostream & out);
	// write to file called outname
	void write(std::string outname);
private:

	double L;		// box size. Use pbc if L>0,
					// if L<=0 don't use pbc.
	unsigned int Nbin;	// number of bins
	double bs;		// bin size

	std::vector<double> bins;	// mid values of the bins 
	std::vector<double> rho;		// pair correlation function
	
	unsigned int Nsamp;	// number of samples
	bool is_normalized;	// if true, pc is normalized;
						// sampling is no longer possible.
};

#endif
