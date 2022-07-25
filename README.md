#MathGrapher


##_Description_

The MathGrapher is a simple program that draws graphs of math functions.

##_Building_

Project can be easily built using

`make`

in root directory of project. Binary file "mathgrapher" will be located in folder bin. You can run it directly or use

`make run`

##_Usage_

1. Functions and variable values can be defined as command line parameters. Run

`make test`

to see an example.

Arguments syntax:

`mathgrapher y=function1 y=function2 a=value1 b=value2 c=value3`

NOTE: Arguments and functions must be written without spaces and you need to use '\\' before parentheses. There is an example from Makefile:

`bin/mathgrapher y=2*sin\(x/2\) y=a+b*x a=1 b=2`

Arguments can be written in any order. You can define only 2 functions

2. Functions and variables 'a' and 'b' can be defined or redefined in GUI control panel. Just start MathGrapher and press 'E'

##_Limitations and troubles_

1. Maximum zoom is 500000000 and renderer feels good on it only while you are looking near the 0. As farther you move - as more laggy will be rendering at large zoom.

2. Infinity jumps leave vertical lines. I've commented code which is written in an attempt to solve this problem.
