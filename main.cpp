
#include "xyz.h"
#include "potential.h"
#include "system.h"
#include "configFile.h"


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

    Potential potential(A,alpha, rhs, rc);
    System system(seed, Ninit, L, Lwall, potential, d, rv);


    for(int t=0; t<T_init; ++t) {

        if(t%print_every == 0) {
            cout <<T+T_init << '\t' <<  t << endl;
            cout << system.Nacc/( (double) system.Ntry ) << endl << endl;
        }

        for(int tmc=0; tmc<Tmc; ++tmc) {
            //system.mc_move();
            system.mc_move_verlet();
        }
    }
    
    
    for(int t=0; t<T; ++t) {

        if(t%print_every == 0) {
            cout <<T+T_init << '\t' <<  t+T_init << endl;
            cout << system.Nacc/( (double) system.Ntry ) << endl << endl;
        }

        for(int tmc=0; tmc<Tmc; ++tmc) {
            //system.mc_move();
            system.mc_move_verlet();
        }

        //if( t%T_sample == 0)
            // sample
    }
    cout <<( (double) system.Nacc)/( (double) system.Ntry) << endl;

    cout << system.Nverlet << endl;
    cout << system.Nacc/( (double) system.Ntry ) << endl;


    return 0;
}
