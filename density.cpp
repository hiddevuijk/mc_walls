
#include "density.h"



Density::Density(double LL, unsigned int Nbinn, double bss)
    :
		L(LL),  Nbin(Nbinn), bs(bss), bins(Nbinn),
        rho(Nbinn,0.), Nsamp(0),is_normalized(false)
{

	// set mid points of the bins
	for(unsigned int i=0;i<Nbin;++i)
		bins[i] = (0.5+i)*bs;
}			

void Density::sample( const std::vector<Particle> &particles, unsigned int N )
{

	// only sample before normalization 
	if(is_normalized){
		// quit
	}

	double d;

	for(unsigned int i=0;i<N;++i) {
        //d = xyz::dist_pbc(particles[i].r,particles[j].r,L) - 1;
        d = particles[i].r.x;
        if(d< 1.*L) rho[ (int)(d/bs) ] += 1 ;
	}

	++Nsamp;

}

void Density::write(std::ostream & out)
{

    double norm = 1./(Nsamp*L*L*bs);
	for(unsigned int i=0;i<Nbin;++i) {
		out << bins[i] << ' ';
		out << rho[i]*norm << '\n';
	}

}


void Density::write(std::string outname)
{
    double norm = 1./(Nsamp*L*L*bs);

	std::ofstream out;
	out.open(outname);
	for(unsigned int i=0;i<Nbin;++i) {
		out << bins[i] << ' ' << rho[i]*norm;
		if( i < Nbin-1) out <<  '\n';
	}
	out.close();
	
}
