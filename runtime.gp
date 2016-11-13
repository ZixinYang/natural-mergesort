reset
set ylabel 'time(sec)'
set style fill solid
set title 'orig vs __ builtin_ clz'
set datafile separator ","
set term png enhanced font 'Verdana,10'
set output 'runtime.png'

plot [:][:0.4]'output.txt' using 1:xtic(1) with histogram title 'original',\
'' using ($0-0.06):($1+0.001):1 with labels title ' ',\
'' using 2:xtic(1) with histogram title 'optimized',\
'' using ($0+0.3):($2+0.0015):2 with labels title ' '

