# LcVRP optimizer

This project is an entry into a contest for a LcVRP problem optimizer. The solver uses a multithreaded genetic algorithm, with parametrizable multi-population evolution.

ProblemData and ProblemLoader, were downloaded from here `https://kn.gangstas.pl/materialy.php`. These classes were slightly modified. Problem data can also be downloaded from the same link.

No AI was used to make this project.

## Compiling

To compile the code run

```
git clone <>
cd LcVRP-optimizer
cmake . -B build
cd build
make
```

## Running the program

The executable `optimizer <data-path>` can be called with the following arguments:
- `-i <int>`, `--iterations <int>` - The number of iterations to run. Default: `100`
- `-t <int>`, `--threads <int>` - The number of threads to use. Default: `8`
- `-p <int>`, `--population-size <int>` - The number of individuals in each population. Default: `100`
- `-c <float>`, `--crossover-p <float>` - The propability of crossovers. Default: `0.7`
- `-m <float>`, `--mutation-p <float>` - The propability of mutations. Default: `0.01`
- `--populations <int>` - The number of separated populations. Default: `4`
- `--transfer-period <int>` - The period of time in iterations between transfers of top individuals between populations. Default: `100`
- `-q`, `--quiet` - Silence additional statistics
- `-b`, `--benchmark` - Measure the time per iteration
