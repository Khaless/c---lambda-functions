
set terminal x11
#set output 'file.jpg'

set xtics ("-SO" 0, "-S1" 1, "-S2" 2, "-S3" 3)

set ylabel "Microseconds for 1,000,000 Executions"
set xrange[-1:4]

set boxwidth 0.2

set logscale y

plot 'plot.dat' using ($1-0.3):2:3 with boxerrorbars fs solid 1 t "Lambda"
replot 'plot.dat' using ($1-0.1):4:5 with boxerrorbars fs solid 2 t "operator()"
replot 'plot.dat' using ($1+0.1):6:7 with boxerrorbars fs solid 3 t "bind2nd()"
replot 'plot.dat' using ($1+0.3):8:9 with boxerrorbars fs solid 4 t "Boost::Lambda"

pause 5
