#include "system.h"

/*
    change pbc to pbc in yz
    remove N > 2 cond. in remove
*/


void System::mc_rand()
{
    double r = rudist01();
    if( r < 1./3 ) {
        mc_add();
    } else if( r > 2./3. ) {
        if( N > 2 ) mc_remove();
    } else {
        mc_move();
    }
}

void System::mc_add()
{


    // random displacement
    XYZ new_position( Lwall*rudist01(), L*rudist01(), L*rudist01() );
    
    Ntry += 1;

    bool wall_overlap = potential.get_wall_overlap(new_position);
    if(!wall_overlap) {
        double deltaU = 0;
        bool overlap = false;
        double new_dist;
        for(unsigned int ni = 0; ni<N; ++ni) {

            new_dist = xyz::dist_pbcYZ(new_position, particles[ni].r, L);

            overlap = potential.get_overlap(new_dist);
            if( overlap ) break;

            deltaU += potential.U(new_dist);
        }

        if( !overlap ) {
            double V = L*L*Lwall;
            if( rudist01() < std::exp( -deltaU + mu )*V/(N+1) ) {
                particles[N].r = new_position;
                ++N;

                // apply periodic boundary conditions
                //particles[i].r.pbc(L);
                Nacc += 1;
            } 
        }

    }
}
void System::mc_remove()
{


    // random displacement
    unsigned int i = (int)(N*rudist01()); 

    Ntry += 1;

    double deltaU = 0;
    double dist;
    for(unsigned int ni = 0; ni<N; ++ni) {
        if(ni == i ) continue;

        dist = xyz::dist_pbcYZ(particles[i].r, particles[ni].r, L);


        deltaU -= potential.U(dist);
    }

    double V = L*L*Lwall;
    if( rudist01() < std::exp( -deltaU - mu )*N/V ) {
        particles[i] = particles[N-1];
        --N;

        // apply periodic boundary conditions
        //particles[i].r.pbc(L);
        Nacc += 1;
    } 

}



// make a move, and check all particles
void System::mc_move()
{

    // random particle index 
    unsigned int i = (int)(N*rudist01()); 

    // random displacement
    XYZ dr( d*rudist11(), d*rudist11(), d*rudist11() );
    XYZ new_position = particles[i].r + dr;
        
    bool wall_overlap = potential.get_wall_overlap(new_position);
    if( wall_overlap) return ;
    Ntry += 1;

    double deltaU = 0;
    bool overlap = false;
    double new_dist, old_dist;

    for(unsigned int ni = 0; ni<N; ++ni) {

        if( ni == i ) continue;

        old_dist = xyz::dist_pbcYZ(particles[i].r, particles[ni].r, L);
        new_dist = xyz::dist_pbcYZ(new_position, particles[ni].r, L);

        overlap = potential.get_overlap(new_dist);
        if( overlap ) break;

        deltaU += potential.U(new_dist) - potential.U(old_dist) ;
    }

    if( !overlap ) {
        if( rudist01() < std::exp( -deltaU ) ) {
            particles[i].r = new_position;

            // apply periodic boundary conditions
            //particles[i].r.pbc(L);
            Nacc += 1;
        } 
    }
}

// make a move and check particles in the Verlet list
void System::mc_move_verlet()
{

    // random particle index 
    unsigned int i = u_int_dist(rng); 

    // random displacement
    XYZ dr( d*rudist11(), d*rudist11(), d*rudist11() );
    XYZ new_position = particles[i].r + dr;


    Ntry += 1;

    double deltaU = 0;
    bool overlap = false;
    double new_dist, old_dist;


    for(unsigned int ni = 0; ni < neighbour_number_list[i] ; ++ni) {

        int neighbour_index = verletList[i][ni];
        old_dist = xyz::dist_pbc(particles[i].r, particles[neighbour_index].r, L);
        new_dist = xyz::dist_pbc(new_position, particles[neighbour_index].r, L);

        overlap = potential.get_overlap(new_dist);
        if( overlap ) break;

        deltaU += potential.U(new_dist) - potential.U(old_dist) ;
    }

    if( !overlap ) {

        if( deltaU < 0 ) { 
            particles[i].r = new_position;

            // apply periodic boundary conditions
            //particles[i].r.pbc(L);
            Nacc += 1;

            // check if the Verlet list needs to be updated
            double dist = xyz::dist_pbc( particles[i].r, particles_before_update[i].r, L);
            if( dist > max_diff ) {
                update_verlet_list();
            }

        } else if( rudist01() < std::exp( -deltaU ) ) {
            particles[i].r = new_position;

            // apply periodic boundary conditions
            //particles[i].r.pbc(L);
            Nacc += 1;

            // check if the Verlet list needs to be updated
            double dist = xyz::dist_pbc( particles[i].r, particles_before_update[i].r, L);
            if( dist > max_diff ) {
                update_verlet_list();
            }
        }
   } 
}

System::System(int seed, unsigned int Ninit,double mu, double L, double Lwall, Potential potential, double d, double rv)
: udist11(-1,1), udist01(0,1), u_int_dist(0,N-1), seed(seed),rng(seed), 
    rudist11(rng,udist11), rudist01(rng,udist01),
    N(Ninit),mu(mu), L(L), Lwall(Lwall), potential(potential),  d(d), rv(rv),
    particles(2*Ninit), particles_before_update(2*Ninit),
    verletList(N, std::vector<int>(2*Ninit) ), neighbour_number_list(2*Ninit)
{
    Ntry = 0;
    Nacc = 0;
    Nverlet = 0;

    max_diff = 0.5*(rv - potential.rco) - sqrt(3.*d*d);

    // initialize the particle on a square
    init_random();
    //init_random_shift();

    // initialize Verlet list
    update_verlet_list();
    

}

void System::update_verlet_list()
{
    Nverlet += 1;

    // set neighbour number to 0
    std::fill(neighbour_number_list.begin(),
                neighbour_number_list.end(), 0);

    for(unsigned int i=0; i<N; ++i) {
        particles_before_update[i] = particles[i];
        for(unsigned int j=i+1; j<N; ++j) {
            if( xyz::dist_sq_pbc(particles[i].r, particles[j].r,L) < rv*rv ){
                verletList[i][ neighbour_number_list[i] ] = j;
                verletList[j][ neighbour_number_list[j] ] = i;
                ++neighbour_number_list[i];
                ++neighbour_number_list[j];
            }
        }
    }
}

void System::init_random()
{
	// node_pd: nodes per dim
	int node_pd = ceil(pow(1.*N,1./3));
	int Nnodes = node_pd*node_pd*node_pd;
	double  node_dist = L/node_pd;

	std::vector<XYZ> nodes(Nnodes);
	int i=0;
	for(int xi =0;xi<node_pd;++xi) {
		for(int yi=0;yi<node_pd;++yi) {
			for(int zi=0;zi<node_pd;++zi) {
				nodes[i].x = xi*node_dist;
				nodes[i].y = yi*node_dist;
				nodes[i].z = zi*node_dist;
				++i;
			}
		}
	}
    
    // shuffle nodes
    for(unsigned int i=0; i<N; ++i) {
        int j = i + (int)( rudist01()*(N-i) );
        XYZ temp = nodes[i];
        nodes[i] = nodes[j];
        nodes[j] = temp;
    }
	for(unsigned  int i=0;i<N; ++i) {
        particles[i].r = nodes[i];
        particles[i].index = i;
        particles_before_update[i] = particles[i];
    }

}
void System::init_random_shift()
{
	// node_pd: nodes per dim
	int node_pd = ceil(pow(1.*N,1./3));
	int Nnodes = node_pd*node_pd*node_pd;
	double  node_dist = L/node_pd;

	std::vector<XYZ> nodes(Nnodes);
	int i=0;
	for(int xi =0;xi<node_pd;++xi) {
		for(int yi=0;yi<node_pd;++yi) {
			for(int zi=0;zi<node_pd;++zi) {
                if( zi%2 == 0 ) {
                    nodes[i].x = xi*node_dist;
                    nodes[i].y = yi*node_dist;
                    nodes[i].z = zi*node_dist;
                } else {
                    nodes[i].x = (xi+0.5)*node_dist;
                    nodes[i].y = (yi+0.5)*node_dist;
                    nodes[i].z = zi*node_dist;
                }
				++i;
			}
		}
	}
    
    // shuffle nodes
    for(unsigned int i=0; i<N; ++i) {
        int j = i + (int)( rudist01()*(N-i) );
        XYZ temp = nodes[i];
        nodes[i] = nodes[j];
        nodes[j] = temp;
    }
	for(unsigned  int i=0;i<N; ++i) {
        particles[i].r = nodes[i];
        particles[i].index = i;
        particles_before_update[i] = particles[i];
    }

}



