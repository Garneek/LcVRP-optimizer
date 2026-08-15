#pragma once

#include <string>

class OptimizerParams {
public:
  OptimizerParams() {}
  OptimizerParams(int argc, const char *argv[]);

  int ITERATIONS = 500;
  int MAX_THREADS = 8;

  int N_THREADS_PER_POP = 2;
  int NUM_POPULATIONS = 4;

  int TRANSFER_PERIOD = 100;

  int POP_SIZE = 100;
  int TOURNAMENT_K = 2;

  int REMOVE_REPEATS_PERIOD = 50;

  double CROSS_P = 0.7;
  double CROSS_GENE_P = 0.2;

  double MUT_P = 0.01;
  double MUT_PERC = 1.0;

  int N_ITERS_LS = 4;

  double GEN_MUT_PERC = 0.5;

  double GEN_MUT_TEMPERATURE = 0.15;
  double TEMP = 0.5;
  double TEMP_MULT = 0.95;

  static const int STRATEGIES = 4;
  double CUMULATIVE_LS_STRAT_PROPABLITIES[STRATEGIES] = {0.05, 0.5, 0.55, 1.0};

  bool QUIET = false;
  bool BENCHMARK = false;

private:
  void ParseParameter(const std::string &param_name, const std::string &value);
  bool RequiresValue(const std::string &param_name) const;

  float ParseFloat(const std::string &param_name,
                   const std::string &value) const;
  int ParseInt(const std::string &param_name, const std::string &value) const;
};