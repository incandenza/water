public class ArrayOutOfBounds {
  public static void main(String args[]) {
    byte[] byte_array = new byte[2];

    byte_array[0] = 0;
    byte_array[1] = 0;
    byte_array[2] = 0;
    byte_array[3] = 0;

    System.out.println("hi.");
  }
}
