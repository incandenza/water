public class CurrentBreakage {
  public static void main(String args[]) {
    try {
      double dvalue1 = 4.2;
      double dvalue2 = (double)dvalue1;
      if (dvalue1 != dvalue2)
	System.out.println("FAILED: " + dvalue1 + " != " + dvalue2);
      else
	System.out.println("PASSED: " + dvalue1 + " == " + dvalue2);
    }
    catch (Exception e)
      {
	System.out.println("FAILED: " + " : exception " + e.toString());
	e.printStackTrace();
      }
  }
}
