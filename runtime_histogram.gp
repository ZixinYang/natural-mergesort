reset
set ylabel 'time(sec)'
set style fill solid
set title 'orig vs continuum detect'
set datafile separator ","
set term png enhanced font 'Verdana,10'
set output 'runtime_histogram.png'

plot [:][0:0.4]'output.txt' using 2:xtic(1) with histogram title 'original',\
'' using ($0-0.06):($1+0.001):2 with labels title ' ',\
'' using 3:xtic(1) with histogram title 'optimized',\
'' using ($0+0.15):($2+0.0015):3 with labels title ' ',\
'' using 4:xtic(1) with histogram title 'opt + segment detect',\
'' using ($0+0.3):($3+0.002):4 with labels title ' ',\

