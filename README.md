# neural_networks_with_gnuplot
C codes for learning how neural nets work

=============

<img src="https://github.com/date333cs/neural_networks_with_gnuplot/blob/master/anim-som2d.gif" height="220px" align="left">

Compile each code by gcc *.c -lm and run and see !!! 
- If gnuplot is installed in your machine, it must work. 
There is no document but all codes are small (less than 200 lines).
So if you want to know how it works, it is not difficult to read the codes.


#### SOM (1 dimension)

- [somanim-1d_001.c](somanim-1d_001.c)
- [kurata-special002.c](kurata-special002.c) (input space: 3 dimension)

#### SOM (2 dimension)

- [som-2d_001.c](som-2d_001.c) [animation]
- [somanim-2d_001.c](somanim-2d_001.c)

#### Associative Memory

- [asmemory090501.c](asmemory090501.c)
- data: [kanji1.h](kanji1.h)

#### A learning of single unit

- [lab070420a.c](lab070420a.c)
- [lab090423p.c](lab090423p.c)
- data: [apples.dat](apples.dat), [oranges.dat](oranges.dat)

#### Backpropagation

- [lab060518b.c](lab060518b.c)
- data: [apples3.dat](apples3.dat), [oranges3.dat](oranges3.dat)