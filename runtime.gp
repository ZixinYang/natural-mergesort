reset
set ylabel 'time(sec)'
set style fill solid
set title 'orig vs __builtin_clz'
set datafile separator ","
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot 'output.txt' using 1:xtic(1) with histogram title 'original',\
'' using ($0-0.06):($2+0.001):2 with labels title ' ',\
'' using 2:xtic(1) with histogram title 'optimized',\
'' using ($0+0.3):($3+0.0015):3 with labels title ' '

