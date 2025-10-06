public class PrimitiveClass {
  public static void main(String args[]) {
    try {
      /*      Class byte_class = Class.forName("byte");*/
      Class byte_class = Byte.TYPE;
      
      System.out.println("byte class's name is " + byte_class.getName());
    } catch(Exception e) {
      System.out.println("got an exception: " + e.getMessage());
    }
  }
}
