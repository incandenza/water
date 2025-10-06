public class ArrayInstance {
  public static void main(String args[]) {
    try {
      byte[] byte_array = new byte[1];
      Class byte_array_class;
      Object new_array;
      byte[] byte_array_2;
      
      byte_array_class = byte_array.getClass();
      new_array = byte_array_class.newInstance();
      byte_array_2 = (byte [])new_array;
      
      byte_array_2[0] = 1;
      byte_array_2[1] = 2;
      byte_array_2[2] = 3;
      
      System.out.println("byte_array_2's values are " +
			 byte_array_2[0] + ", " +
			 byte_array_2[1] + ", " +
			 byte_array_2[2]);
    } catch(Exception e) {
      System.out.println("got an exception: " + e.getMessage());
      e.printStackTrace();
    }
  }
}
