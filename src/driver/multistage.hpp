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

#ifndef MULTISTAGE_HPP
#define MULTISTAGE_HPP

#include <string>
#include <vector>

#include "driver/driver.hpp"
#include "mesh/mesh.hpp"
#include "parameter_input.hpp"

namespace parthenon {

struct Integrator {
  Integrator() = default;
  Integrator(int nstages, std::vector<Real> beta) : _nstages(nstages), _beta(beta) {}
  int _nstages;
  std::vector<Real> _beta;
};

class MultiStageDriver : public EvolutionDriver {
 public:
  MultiStageDriver(ParameterInput *pin, Mesh *pm, Outputs *pout);
  std::vector<std::string> stage_name;
  Integrator *integrator;
  ~MultiStageDriver() { delete integrator; }

 private:
};

class MultiStageBlockTaskDriver : public MultiStageDriver {
 public:
  MultiStageBlockTaskDriver(ParameterInput *pin, Mesh *pm, Outputs *pout)
      : MultiStageDriver(pin, pm, pout) {}
  TaskListStatus Step();
  virtual TaskList MakeTaskList(MeshBlock *pmb, int stage) = 0;
};

} // namespace parthenon

#endif
