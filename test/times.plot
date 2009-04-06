set terminal png
set output 'file.png'

set xtics ("-OO" 0, "-O1" 1, "-O2" 2, "-O3" 3)

set ylabel "Microseconds"
set xrange[-0.8:3.8]

set title "Time vs Optimisation - Execution over a collection of 1,000,000 integers"

set boxwidth 0.16

#set logscale y

plot 'plot.dat' using ($1-0.32):2:3 with boxerrorbars fs solid 1 t "My Lambda", \
		 'plot.dat' using ($1-0.16):4:5 with boxerrorbars fs solid 2 t "Object \\w Functor", \
		 'plot.dat' using ($1+0.0):6:7 with boxerrorbars fs solid 4 t "unary_function", \
		 'plot.dat' using ($1+0.16):8:9 with boxerrorbars fs solid 3 t "<functional>", \
		 'plot.dat' using ($1+0.32):10:11 with boxerrorbars fs solid 4 t "Boost::Lambda"
