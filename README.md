# CS315_MonteCarloTreeSearch

Download gnuplot from https://sourceforge.net/p/gnuplot/gnuplot-main/merge-requests/
or `brew install gnuplot` or `sudo apt-get install gnuplot` or whatever

In main.c, change the line:
`FILE *gnuplotPipe = popen("/opt/homebrew/bin/gnuplot -persistent", "w");`
to reflect the location of the gnuplot binary on your system

Build with:
`sh run.sh`

Please use the src folder for any source files (example.c, example.h, etc.)

