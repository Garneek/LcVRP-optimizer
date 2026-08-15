#pragma once

#include "ProblemData.hpp"
#include <string>

using namespace std;

namespace LcVRPContest {
class ProblemLoader {
public:
  ProblemLoader(const string &folder_name, bool use_random_permutation = true);

  ProblemData LoadProblem();

private:
  string file_path_;
  bool use_random_permutation_;

  void ParseLcVrpFile(const string &file_path, ProblemData &problem_data);
  void ParseEdgeWeightSection(ifstream &file, ProblemData &problem_data);
  void ParseNodeCoordSection(ifstream &file, ProblemData &problem_data);
  void ParseDemandSection(ifstream &file, ProblemData &problem_data);
  void ParseDepotSection(ifstream &file, ProblemData &problem_data);
};
} // namespace LcVRPContest
