reset
set xlabel 'segment size'
set ylabel 'time(sec)'
set style fill solid
set title 'segment size analysis'
set datafile separator ","
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [5:10][0.1:0.25]'output.txt' using 1:2 title 'optimized' with linespoints,\
'' using ($0-0.06):($1+0.001):0.2 with labels title ' '
