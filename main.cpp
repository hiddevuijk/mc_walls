
#include "xyz.h"
#include "potential.h"
#include "system.h"
#include "configFile.h"
#include "density.h"


#include <vector>
#include <iostream>

using namespace std;

int main()
{

	Config config("input.txt");

    int T = config.get_parameter<int>("T");
    int T_init = config.get_parameter<int>("T_init");
    int T_sample = config.get_parameter<int>("T_sample");
    int Tmc = config.get_parameter<int>("Tmc");
    int print_every = config.get_parameter<int>("print_every");

    int seed = config.get_parameter<int>("seed");
    int Ninit = config.get_parameter<int>("Ninit");
    double mu = config.get_parameter<double>("mu");
    double L = config.get_parameter<double>("L");
    double Lwall = config.get_parameter<double>("Lwall");
    double d = config.get_parameter<double>("d");

    double rhs = config.get_parameter<double>("rhs");
    double rc = config.get_parameter<double>("rc");
    double A = config.get_parameter<double>("A");
    double alpha = config.get_parameter<double>("alpha");
    double rv = config.get_parameter<double>("rv");

    int Nbin = config.get_parameter<int>("Nbin");

    double bs = (1.*L)/(1.*Nbin);

    Potential potential(Lwall, A,alpha, rhs, rc);
    System system(seed, Ninit,mu, L, Lwall, potential, d, rv);
    Density density(Lwall, Nbin, bs);


    for(int t=0; t<T_init; ++t) {

        if(t%print_every == 0) {
            cout << system.N << endl;
            cout <<T+T_init << '\t' <<  t << endl;
            cout << system.Nacc << endl;
            cout << system.Nacc/( (double) system.Ntry ) << endl << endl;
            system.Nacc = 0;
            system.Ntry = 0;
        }

        for(int tmc=0; tmc<Tmc; ++tmc) {
            system.mc_rand();

            //system.mc_add();
            //// change to 0
            //if(system.N>1) system.mc_remove();
            //
            //system.mc_move();
            ////system.mc_move_verlet();
        
        }
    }
    

    for(int t=T_init; t<(T+T_init); ++t) {

        if(t%print_every == 0) {
            cout << system.N << endl;
            cout <<T+T_init << '\t' <<  t << endl;
            cout << system.Nacc << endl;
            cout << system.Nacc/( (double) system.Ntry ) << endl << endl;
            system.Nacc = 0;
            system.Ntry = 0;
        }

        for(int tmc=0; tmc<Tmc; ++tmc) {
            system.mc_rand();
    
            //system.mc_add();
            //// change to 0
            //if(system.N>1) system.mc_remove();
            //system.mc_move();
            ////system.mc_move_verlet();
        
        }
        
        density.sample(system.particles, system.N);
    }
    cout << system.Nacc / (double) system.Ntry << endl; 
    cout <<  system.Ntry << endl; 
    density.write("rho.dat");
    return 0;
}
