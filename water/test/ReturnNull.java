public class ReturnNull {
  public static String return_null() {
    return null;
  }

  public static void main() {
    String a = return_null();
    String b = null;
    byte[] bytes = null;

    System.out.println(a);
    System.out.println(b);
    System.out.println(bytes.length);
  }
};
