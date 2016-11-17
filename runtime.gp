reset
set ylabel 'time(nanosec)'
set style fill solid
<<<<<<< HEAD
set title 'natural-mergesort runtime'
=======
set title 'orig vs cmp_inline'
>>>>>>> a06531e88a7e91911fa83fc05ace9a86b080e863
set datafile separator ","
set term png enhanced font 'Verdana,10'
set xtics rotate by -45
set output 'runtime.png'

<<<<<<< HEAD

plot [:][:]'output_final.txt' using 2:xtic(1) with histogram title 'original',\
'' using ($0-0.06):($1+0.001):2 with labels title ' ',\
'' using 3:xtic(1) with histogram title 'optimized',\
'' using ($0+0.15):($2+0.0015):3 with labels title ' '\
=======
plot [:][0:0.6]'output.txt' using 2:xtic(1) with histogram title 'original',\
'' using ($0-0.06):($2+0.001):2 with labels title ' ',\
'' using 3:xtic(1) with histogram title 'optimized',\
'' using ($0+0.3):($3+0.0015):3 with labels title ' ',
>>>>>>> a06531e88a7e91911fa83fc05ace9a86b080e863
