#include <arrayfire.h>
#include <af/defines.h>
#include <af/dim4.hpp>
#include <af/traits.hpp>
//#include <Accelerate/Accelerate.h>
#include </opt/intel/compilers_and_libraries_2020.0.166/mac/mkl/include/mkl.h>
#include <iostream>







struct MNASolver
{

    int nets;
    int rNets;
    int maxIter = 200;
    double tStep;
    double **systemA;
    double *systemA_lu;
    int *pivot;
    double **systemB;
    double *systemX;
    double *r;
    double *c;
    double *work;
    int *iwork;
    float rcond;
    float ferr;
    float berr;
    int info;
    char e = 'E';
    char n = 'N';
    char set;
    int zero = 0;
    int one = 1;
    float room;
    /*
      MNASolver(int size) : nets(size-1)
      {


      } */


public:

    void setSize(int size, double timestep, MNASystem & m)
    {

        nets = size;
        rNets = size-1;
        tStep = timestep;

         systemA = new double*[rNets*rNets];
         systemA_lu = new double[rNets*rNets];
         pivot = new int[rNets*rNets];
         systemB = new double*[rNets];
         //systemX = new double*[rNets];
         r = new double[rNets];
         c = new double[rNets];
         work = new double[rNets*rNets];
         iwork = new int[rNets*rNets];

         for (int i = 0; i < rNets; i++ ) {
             systemB[i] = &m.b[i+1].lu;
             for (int j = 0; j < rNets; j++ ) {
                 systemA[(i*(rNets))+j] = &m.A[j+1][i+1].lu; //Fill a 1D array with pointers to the 2D matrix, because lapack requires a 1d matrix
             }
         }


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
          //std::cout << "oude iter "<< iter << '\n';
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




    void solve4(MNASystem & m)
        {

          /*
          std::cout << "heyyy" << '\n';

          double systemA[rNets*rNets];
          double systemB[rNets];
          double systemX[rNets];

          long colstarts[rNets];
          int rowind[rNets];

          for (size_t i = 0; i < rNets; i++) {
            colstarts[i] = i*rNets;
            rowind[i] = i;
          }

          SparseMatrixStructure astruc;
          astruc.rowCount = rNets;
          astruc.columnCount = rNets;
          astruc.columnStarts = colstarts;
          astruc.attributes = SparseAttributes_t();
          astruc.blockSize = 1;
          astruc.rowIndices = rowind;


          updatePre(tStep, m);





          for (int i = 0; i < rNets; i++ ) {
              systemB[i] = m.b[i+1].lu;
              for (int j = 0; j < rNets; j++ ) {
                  systemA[(i*(rNets))+j] = m.A[j+1][i+1].lu; //klopt dit??
              }
          }
              SparseMatrix_Double a;
              //a.withUnsafeMutableBufferPointer();
              a.structure = astruc;
              a.data = systemA;


              DenseMatrix_Double b;
              b.data = systemB;

              DenseMatrix_Double x;
              x.data = systemX;


              bool status = SparseSolve(SparseLSMR(), a, b, x);


              m.b[0].lu = 0;
              for (size_t i = 1; i < nets; i++) {
                  m.b[i].lu = systemX[i-1];

              }
              //dgesvx();

              //cblas_dgesvx_();
              /*
              cblas_dgesvx(CblasRowMajor, CblasNoTrans, 8, 4, 1.0f, (*)a, 4, x, 1, 1.0f, y, 1);

              std::vector<double> a1(systemA);
              std::vector<double> b1(systemB);
              std::vector<int> ipiv(rNets);


              cblas_dgesv_('N', 'N', 0, 1, &systemA, &rNets, &rNets, a1.data(), &dim, ipiv.data(), b1.data(), &dim, &info);

              dgesv( rNets, int* nrhs, double* a, int* lda, int* ipiv,
                double* b, int* ldb, int* info );

              cblas_dgesv_(&rNets, &rNets, a1.data(), &dim, ipiv.data(), b1.data(), &dim, &info); */

        }


        /*

        int dgesvx_(
          char *__fact,
          char *__trans,
          int *__n,
          int *__nrhs,
           *__a,
          int *__lda,
           *__af,
          int *__ldaf,
          int *__ipiv,
          char *__equed,
           *__r__,  *__c__,
                 *__b, int *__ldb,  *__x,
                int *__ldx, double *__rcond,
                double *__ferr, double *__berr,
                double *__work, int *__iwork,
                int *__info) */


        void solve5(std::vector<IComponent*> &components, MNASystem & m)
        {

                  int iter;

                  for(iter = 0; iter < maxIter; ++iter)
                  {

                    updatePre(tStep, m);

                      dgesv_(&rNets, &one, *systemA, &rNets, pivot, *systemB, &rNets, &info);

                      if(newton(components, m)) break;

                      e = 'E';
                }

                  //e = 'F';
      }


      void solve6(std::vector<IComponent*> &components, MNASystem & m)
      {

                int iter;

                for(iter = 0; iter < maxIter; ++iter)
                {

                  updatePre(tStep, m);




                    //sposv_( "Lower", &rNets, &one, systemA, &rNets, systemB, &rNets, &info );

                     //la_solve();


                    //std::cout << "nieuwe iter "<< iter << '\n';

                    if(newton(components, m)) break;

                    e = 'E';
              }

                e = 'F';
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
