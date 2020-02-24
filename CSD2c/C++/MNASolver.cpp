#include <arrayfire.h>
#include <af/defines.h>
#include <af/dim4.hpp>
#include <af/traits.hpp>

#include <iostream>


struct MNASolver
{

  int nets;
  int rNets;
  double tStep;

/*
  MNASolver(int size) : nets(size-1)
  {


  } */


public:

  void setSize(int size, double timestep)
  {

    nets = size;
    rNets = size-1;
    tStep = timestep;

  }


  void solve(MNASystem & m)
  {

    updatePre(tStep, m);


    std::vector<double> systemA((rNets)*(rNets));
    std::vector<double> systemB(rNets);

    for (int i = 0; i < rNets; i++ ) {
      systemB[i] = m.b[i+1].lu;
      for (int j = 0; j < rNets; j++ ) {
        systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu; //klopt dit??
      }
    }

  af::array A(rNets, rNets, f64);
  af::array b(rNets, f64);

  A.write(static_cast<void*>(systemA.data()), systemA.size() * sizeof(double));
  b.write(static_cast<void*>(systemB.data()), systemB.size() * sizeof(double));

  af::array x = af::solve(A, b);


  double *host_a = new double[rNets*rNets];
  double *host_x = new double[rNets];

  A.host(host_a);
  x.host(host_x);


  m.b[0].lu = 0;
  for (size_t i = 1; i < rNets; i++) {
    m.b[i].lu = host_x[i-1];

  }

  delete [] host_a;
  delete [] host_x;

  }


  void solve2(MNASystem & m)
  {
    updatePre(tStep, m);


    std::vector<double> systemA((rNets)*(rNets));
    std::vector<double> systemB(rNets);

    for (int i = 0; i < rNets; i++ ) {
      systemB[i] = m.b[i+1].lu;
      for (int j = 0; j < rNets; j++ ) {
        systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu; //klopt dit??
      }
    }

    af::array A(nets, nets, f64);
    af::array b(nets, f64);

    A.write(static_cast<void*>(systemA.data()), systemA.size() * sizeof(double));
    b.write(static_cast<void*>(systemB.data()), systemB.size() * sizeof(double));


    af::array A_lu, pivot;
    af::lu(A_lu, pivot, A);
    af::array x = af::solveLU(A, pivot, b);


      double *host_a = new double[nets*nets];
      double *host_x = new double[nets];

      A.host(host_a);
      x.host(host_x);


      m.b[0].lu = 0;
      for (size_t i = 1; i < nets; i++) {
        m.b[i].lu = host_x[i-1];

      }

      delete [] host_a;
      delete [] host_x;



  }

  void solve3(std::vector<IComponent*> &components, MNASystem & m)
  {
    int iter;

    for(iter = 0; iter < maxIter; ++iter)
    {
        // restore matrix state and add dynamic values
        updatePre(tStep, m);

        if(nets > 1) {

            luFactor(m);

            luForward(m);

            luSolve(m);

            if(newton(components, m)) break;
        }
    }


  }

protected:

  // return true if we're done
  bool newton(std::vector<IComponent*> &components, MNASystem & m)
  {
      bool done = 1;
      for(int i = 0; i < components.size(); ++i)
      {
          done &= components[i]->newton(m);
      }
      return done;
  }


  void updatePre(double stepScale, MNASystem & m)
  {
      for(int i = 0; i < nets; ++i)
      {
          m.b[i].updatePre(stepScale);
          for(int j = 0; j < nets; ++j)
          {
              m.A[i][j].updatePre(stepScale);
          }
      }
  }

  void luFactor( MNASystem & m)
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
  int luForward( MNASystem & m)
  {
      int p;
      for(p = 1; p < nets; ++p)
      {
          // perform reduction on rows below
          for(int r = p+1; r < nets; ++r)
          {
              if(m.A[r][p].lu == 0) continue;
              if(m.b[p].lu == 0) continue;
              // A.lu bevat 1, 0 of -1. 0 is al uitgesloten. systeem b gaat + of - zichzelf maar wordt daardoor 0
              m.b[r].lu -= m.b[p].lu * m.A[r][p].lu;
          }
      }
      return p;
  }

  // solves nodes backwards from limit-1
  int luSolve( MNASystem & m)
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



};
