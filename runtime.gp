reset
set ylabel 'time(nanosec)'
set style fill solid
set title 'natural-mergesort runtime'
set datafile separator ","
set term png enhanced font 'Verdana,10'
set xtics rotate by -45
set output 'runtime.png'


plot [:][:]'output_final.txt' using 2:xtic(1) with histogram title 'original',\
'' using ($0-0.06):($1+0.001):2 with labels title ' ',\
'' using 3:xtic(1) with histogram title 'optimized',\
'' using ($0+0.15):($2+0.0015):3 with labels title ' '\
