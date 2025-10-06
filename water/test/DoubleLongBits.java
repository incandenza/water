public class DoubleLongBits {
  public static void main(String args[]) {
    double d = 4.5;
    long l = 1;
    long d_as_l;
    double l_as_d;

    d_as_l = Double.doubleToLongBits(d);
    l_as_d = Double.longBitsToDouble(l);

    System.out.println("double " + d + " as long bits is " + d_as_l);
    System.out.println("long bits " + l + " as a double is " + l_as_d);

  }
}
