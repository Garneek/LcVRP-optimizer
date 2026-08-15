#include "FastEvaluator/FastEvaluator.hpp"
#include "Optimizer.hpp"
#include "OptimizerParams.hpp"
#include "ProblemLoader.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <ostream>

using namespace LcVRPContest;

const string HELP_TEXT = //
    "This executable is a lcvrp optimzier. Command structure should be as "
    "follows\n"
    "optimizer <file-path> <args>\n"
    "-i <int> | --iterations <int> \t\t The number of iterations to run. "
    "Defaults "
    "to 500\n"
    "-t <int> | --threads <int> \t\t The maximum number of threads to use. "
    "Defaults to 8\n"
    "-p <int> | --population-size <int> \t The number of individuals in each "
    "population. Defaults to 100\n"
    "-c <float> | --crossover-p <float> \t The propability of crossovers. "
    "Defaults to 0.7\n"
    "-m <float> | --mutation-p <float> \t The propability of mutations. "
    "Defaults to 0.01\n"
    "--populations <int> \t\t\t The number of separated populations. Defaults "
    "to "
    "4\n"
    "--transfer-period <int> \t\t The period of time in iterations between "
    "transfers of top scoring individuals between populations. Defaults to "
    "100\n"
    "-q | --quiet \t\t\t\t Silence additional statistics\n"
    "-b | --benchmark \t\t\t Measure the number of iterations per second\n";

void PrintTimeStatistics(int init, float ms) {
  cout << "initialization time: " << init << "ms" << endl;
  cout << "time per iteration: ";
  cout << ms << "ms" << endl;
}

void PrintDataHeader(const OptimizerParams &params) {
  cout << "N) " << "best fitness\t| ";
  for (int pop_n = 0; pop_n < params.NUM_POPULATIONS; pop_n++)
    cout << "min,\tavg,\tmed\t| ";
  cout << "change\t| temperature";
  cout << endl;
}

void StartOptimization(OptimizerParams &params, const string &instance_path) {

  auto before_loading = chrono::high_resolution_clock::now();

  ProblemLoader problem_loader(instance_path, false);
  ProblemData problem_data = problem_loader.LoadProblem();

  int num_groups = problem_data.GetNumGroups();
  FastEvaluator evaluator(problem_data, num_groups);
  Optimizer optimizer(evaluator, params);

  optimizer.Initialize();

  auto initialization_duration =
      chrono::high_resolution_clock::now() - before_loading;

  auto before_running = chrono::high_resolution_clock::now();

  for (int i = 0; i < params.ITERATIONS; ++i) {
    optimizer.RunIteration();
  }

  auto runtime_duration = chrono::high_resolution_clock::now() - before_running;

  vector<int> *best_solution = optimizer.GetCurrentBest();
  double best_fitness = optimizer.GetCurrentBestFitness();

  cout << "final best fitness: " << best_fitness << endl;
  if (!params.QUIET) {
    cout << "best solution: " << endl;
    for (int v : *best_solution) {
      cout << v << ' ';
    }
    cout << endl;
  }

  if (params.BENCHMARK) {
    int init =
        chrono::duration_cast<chrono::milliseconds>(initialization_duration)
            .count();
    float ms =
        ((float)chrono::duration_cast<chrono::microseconds>(runtime_duration)
             .count() /
         (float)params.ITERATIONS) /
        1000.;
    PrintTimeStatistics(init, ms);
  }
}

int main(int argc, const char *argv[]) try {
  if (argc == 1) {
    std::cout
        << "Provide problem data path or run the program with --help flag";
    return 0;
  }
  string arg_zero(argv[1]);

  if (arg_zero == "--help" || arg_zero == "-h") {
    cout << HELP_TEXT;
  } else {
    OptimizerParams params(argc, argv);

    if (!params.QUIET)
      PrintDataHeader(params);

    StartOptimization(params, arg_zero);
  }

  return 0;
} catch (std::exception &e) {
  std::cout << "Error: " << e.what() << std::endl;
}
