#include "OptimizerParams.hpp"

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <string>

OptimizerParams::OptimizerParams(int argc, const char *argv[]) {
  for (int i = 2; i < argc; i++) {
    const char *p_arg = argv[i];
    std::string arg(p_arg);
    std::string value("");

    if (arg.size() < 2) {
      throw std::invalid_argument("Optimizer parameters not valid");
    } else if (arg[0] == '-') {
      if (RequiresValue(arg)) {
        if (i == argc - 1)
          throw std::invalid_argument("No parameter value provided for " + arg);

        value = argv[++i];
      }

      ParseParameter(arg, value);
    } else {
      throw std::invalid_argument("Optimizer parameters not valid");
    }
  }

  N_THREADS_PER_POP = std::max(MAX_THREADS / NUM_POPULATIONS, 1);
}

void OptimizerParams::ParseParameter(const std::string &param_name,
                                     const std::string &value) {
  if (param_name == "-q" || param_name == "--quiet")
    QUIET = true;
  if (param_name == "-b" || param_name == "--benchmark")
    BENCHMARK = true;
  else if (param_name == "-i" || param_name == "--iterations") {
    ITERATIONS = ParseInt(param_name, value);
    if (ITERATIONS <= 0)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be higher then 0");
  } else if (param_name == "-t" || param_name == "--threads") {
    MAX_THREADS = ParseInt(param_name, value);
    if (MAX_THREADS <= 0)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be higher then 0");
  } else if (param_name == "-p" || param_name == "--population-size") {
    POP_SIZE = ParseInt(param_name, value);
    if (POP_SIZE <= 0)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be higher then 0");
  } else if (param_name == "-c" || param_name == "--crossover-p") {
    CROSS_P = ParseFloat(param_name, value);
    if (CROSS_P < 0 || CROSS_P > 1)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be between 0 and 1");
  } else if (param_name == "-m" || param_name == "--mutation-p") {
    MUT_P = ParseFloat(param_name, value);
    if (MUT_P < 0 || MUT_P > 1)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be between 0 and 1");
  } else if (param_name == "--populations") {
    NUM_POPULATIONS = ParseInt(param_name, value);
    if (NUM_POPULATIONS <= 0)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be higher then 0");
  } else if (param_name == "--transfer-period") {
    TRANSFER_PERIOD = ParseInt(param_name, value);
    if (TRANSFER_PERIOD <= 0)
      throw std::invalid_argument("Parameter " + param_name +
                                  " must be higher then 0");
  }
}

bool OptimizerParams::RequiresValue(const std::string &param_name) const {
  return !(param_name == "-q" || param_name == "--quiet" ||
           param_name == "-b" || param_name == "--benchmark");
}

float OptimizerParams::ParseFloat(const std::string &param_name,
                                  const std::string &value) const try {
  return std::stof(value);
} catch (std::exception &_) {
  throw std::invalid_argument("Parameter value for " + param_name + " invalid");
}

int OptimizerParams::ParseInt(const std::string &param_name,
                              const std::string &value) const try {
  return std::stoi(value);
} catch (std::exception &_) {
  throw std::invalid_argument("Parameter value for " + param_name + " invalid");
}
