/**
 test put/get static opcodes
 exactly one of each get/put (8 total) and one integer compare branch
 per loop
 Compile without -O !!!!
*/

public class TestOpcodeStatic
{
       static int sint;
       static long slong;
       static float sfloat;
       static double sdouble;
       
       public static void main( String argv[] )
       {
	   //               long startTime = System.currentTimeMillis();
               for( sint =0; sint < 1000000; sint++ )
               {
                       slong = slong;
                       sfloat = sfloat;
                       sdouble = sdouble;
               }
	       //               System.out.println(System.currentTimeMillis()-startTime);
       }
}

