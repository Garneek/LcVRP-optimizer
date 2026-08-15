#pragma once

class FastEvaluator;

#include "ProblemData.hpp"

#include <algorithm>
#include <mutex>
#include <vector>

using namespace LcVRPContest;

class FastEvaluator {
private:
  const ProblemData &problem_data;

  int num_groups;
  int num_customers;

  bool valid_data;

  mutex best_fitness_guard;
  double best_fitness;

  vector<vector<double>> distances;
  const double WRONG_VAL = -1.0;

  bool ValidateProblemData();

  void PrecalculateDistances();
  void BuildRoutes(const vector<int> &grouping, vector<vector<int>> &routes);
  double CalculateRouteCost(const vector<int> &route);

public:
  FastEvaluator(const ProblemData &problem_data, int num_groups);

  const vector<vector<double>> &GetDistances() const { return distances; }
  double GetDistance(int i, int j) const {
    return distances[max(i, j)][min(i, j)];
  }

  double Evaluate(const vector<int> &solution);
  double Evaluate(const vector<int> *solution);
  double Evaluate(const int *solution);

  int GetSolutionSize() const { return num_customers; }
  int GetLowerBound() const { return 0; }
  int GetUpperBound() const { return num_groups - 1; }
  int GetNumGroups() const { return num_groups; }
  double GetBestFitness();

  const ProblemData &GetProblemData() const { return problem_data; }
};