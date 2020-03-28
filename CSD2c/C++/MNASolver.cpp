#ifdef __linux__
  #if __has_include("/opt/intel/compilers_and_libraries_2020.0.166/linux/mkl/include/mkl.h")
    #include </opt/intel/compilers_and_libraries_2020.0.166/linux/mkl/include/mkl.h>
  #endif

#elif __APPLE__
  #if __has_include("/opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h")
    #include </opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h>
  #endif
#endif


#include <iosfwd>
#include <cmath>
#include "IComponent.h"
#include "MNASolver.h"


void MNASolver::setSize(int size, MNASystem & m)
{

        // Information the solver needs
        nets = size;
        rNets = size-1;

        // Make space in our matrices
        systemA = new double[rNets*rNets];
        systemB = new double[rNets];
        pivot = new int[rNets*rNets];

}


void MNASolver::setIterations(int iterations)
{

        maxIter = iterations;

}

// Old solver, use this if you don't want to install Intel MKL
// (I did not write this myself!)
void MNASolver::solve(std::vector<IComponent*> &components, MNASystem & m)
{

        for(iter = 0; iter < maxIter; ++iter)
        {
                // restore matrix state and add dynamic values
                updatePre(m);

                if(nets > 1) {

                        luFactor(m);

                        luForward(m);

                        luSolve(m);

                        if(newton(components, m)) break;
                }
        }

}





void MNASolver::solveMKL(std::vector<IComponent*> &components, MNASystem & m)
{

        for(iter = 0; iter < maxIter; ++iter)
        {

                updatePre(m);

                if(nets > 1) {

                        // Convert input format for dgesv
                        for (int i = 0; i < rNets; i++ ) {
                                systemB[i] = m.b[i+1].lu; // move it by 1 because we dont care about the ground row which is always all zeroes -> increases calculation time and means nothing
                                for (int j = 0; j < rNets; j++ ) {
                                        systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu;
                                }
                        }

                        // Solve the system!
                        dgesv_(&rNets, &one, systemA, &rNets, pivot, systemB, &rNets, &info);


                        // Put output values back in the b vector
                        m.b[0].lu = 0;
                        for (size_t i = 1; i < nets; i++) {
                                m.b[i].lu = systemB[i-1];

                        }

                        // Check if junctions are accurate enough
                        if (newton(components, m)) break;

                }
                else {
                        break;
                }
        }

}


// return true if we're done
bool MNASolver::newton(std::vector<IComponent*> &components, MNASystem & m)
{
        bool done = 1;
        for(int i = 0; i < components.size(); ++i)
        {
                done &= components[i]->newton(m);
        }
        return done;
}


void MNASolver::updatePre(MNASystem & m)
{
        for(int i = 0; i < nets; ++i)
        {
                m.b[i].updatePre(m.tStep);
                for(int j = 0; j < nets; ++j)
                {
                        m.A[i][j].updatePre(m.tStep);
                }
        }
}

void MNASolver::luFactor( MNASystem & m)
{
        int p;
        for(p = 1; p < nets; ++p)
        {
                // FIND PIVOT
                {
                        int pr = p;
                        for(int r = p; r < nets; ++r)
                        {
                                if(fabs(m.A[r][p].lu)
                                   > fabs(m.A[pr][p].lu))
                                {
                                        pr = r;
                                }
                        }
                        // swap if necessary
                        if(pr != p)
                        {
                                std::swap(m.A[p], m.A[pr]);
                                std::swap(m.b[p], m.b[pr]);
                        }
                }
                if(0 == m.A[p][p].lu)
                {
                        printf("Failed to find a pivot!!");
                        printf("ERROR: Invalid circuit");
                        throw;
                        return;
                }

                // take reciprocal for D entry
                m.A[p][p].lu = 1 / m.A[p][p].lu;

                // perform reduction on rows below
                for(int r = p+1; r < nets; ++r)
                {
                        if(m.A[r][p].lu == 0) continue;

                        m.A[r][p].lu *= m.A[p][p].lu;
                        for(int c = p+1; c < nets; ++c)
                        {
                                if(m.A[p][c].lu == 0) continue;

                                m.A[r][c].lu -=
                                        m.A[p][c].lu * m.A[r][p].lu;
                        }

                }
        }
}

// this does forward substitution for the solution vector
int MNASolver::luForward( MNASystem & m)
{
        int p;
        for(p = 1; p < nets; ++p)
        {
                // perform reduction on rows below
                for(int r = p+1; r < nets; ++r)
                {
                        if(m.A[r][p].lu == 0) continue;
                        if(m.b[p].lu == 0) continue;
                        m.b[r].lu -= m.b[p].lu * m.A[r][p].lu;
                }
        }
        return p;
}

// solves nodes backwards from limit-1
int MNASolver::luSolve( MNASystem & m)
{
        for(int r = nets; --r;)
        {
                //printf("solve node %d\n", r);
                for(int s = r+1; s < nets; ++s)
                {
                        m.b[r].lu -= m.b[s].lu * m.A[r][s].lu;
                }

                m.b[r].lu *= m.A[r][r].lu;
        }
        return 1;
}
