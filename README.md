# JVM Implementation in C

**Historical Note:** This is a Java Virtual Machine implementation I wrote in C in around 1998. The code is preserved here as a portfolio piece. Build instructions below are original and may not work on modern systems without modification.

Approximately 18,000 lines of C code implementing core JVM functionality.


Original README:

To use this code,

1) Build it by typing 'make'.  Set 'export BUILD_OPT=1' first
   for the optimized version.  (The debugging version has a lot of
   extra output that you probably won't want unless something breaks.)
   NOTE: the code will currently only build under Linux/Intel.

2) Unzip the classes.zip file from Sun's JDK 1.1.5 and put the
   resulting directory in your CLASSPATH.  Classes cannot be pulled
   out of .zip files currently.

3) Now run 'water-opt' or 'water-debug' in this directory with the class
   to run as an argument.  Two possible tests are:

	./water-opt HelloWorld
	./water-opt sun.tools.javac.Main -verbose HelloWorld

   To make 'water-opt' and 'water-debug' work from other directories,
   edit them and change the 'WATER_DIR' variable.

Please let me know of any problems.

Dan McGuirk
<mcguirk@gmail.com>
