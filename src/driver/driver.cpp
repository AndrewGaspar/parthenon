
//========================================================================================
// (C) (or copyright) 2020. Triad National Security, LLC. All rights reserved.
//
// This program was produced under U.S. Government contract 89233218CNA000001 for Los
// Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC
// for the U.S. Department of Energy/National Nuclear Security Administration. All rights
// in the program are reserved by Triad National Security, LLC, and the U.S. Department
// of Energy/National Nuclear Security Administration. The Government is granted for
// itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide
// license in this material to reproduce, prepare derivative works, distribute copies to
// the public, perform publicly and display publicly, and to permit others to do so.
//========================================================================================

#include "driver.hpp"
#include "mesh/mesh.hpp"
#include "outputs/outputs.hpp"
#include "parameter_input.hpp"
#include "utils/utils.hpp"

namespace parthenon {

DriverStatus EvolutionDriver::Execute() {
    pmesh->mbcnt = 0;
    while ((pmesh->time < pmesh->tlim) &&
           (pmesh->nlim < 0 || pmesh->ncycle < pmesh->nlim)) {

        if (Globals::my_rank == 0) pmesh->OutputCycleDiagnostics();

        Step();
        // pmesh->UserWorkInLoop();

        pmesh->ncycle++;
        pmesh->time += pmesh->dt;
        pmesh->mbcnt += pmesh->nbtotal;
        pmesh->step_since_lb++;

        pmesh->LoadBalancingAndAdaptiveMeshRefinement(pinput);

        pmesh->NewTimeStep();
#ifdef ENABLE_EXCEPTIONS
        try {
#endif
            if (pmesh->time < pmesh->tlim) // skip the final output as it happens later
                pouts->MakeOutputs(pmesh, pinput);
#ifdef ENABLE_EXCEPTIONS
        } catch (std::bad_alloc &ba) {
            std::cout << "### FATAL ERROR in main" << std::endl
                      << "memory allocation failed during output: " << ba.what()
                      << std::endl;
#ifdef MPI_PARALLEL
            MPI_Finalize();
#endif
            return (DriverStatus::failed);
        } catch (std::exception const &ex) {
            std::cout << ex.what() << std::endl; // prints diagnostic message
#ifdef MPI_PARALLEL
            MPI_Finalize();
#endif
            return (DriverStatus::failed);
        }
#endif // ENABLE_EXCEPTIONS

        // check for signals
        if (SignalHandler::CheckSignalFlags() != 0) {
            break;
        }
    } // END OF MAIN INTEGRATION LOOP
      // ======================================================
    return DriverStatus::complete;
}

} // namespace parthenon
