import java.io.*;

public class SerializeString {
  public static void main(String[] args) {
    String hi = "hi there";
    FileOutputStream f = null;
    ObjectOutputStream o = null;
    try {
      f = new FileOutputStream("string.out");
      o = new ObjectOutputStream(f);

      o.writeObject(hi);
    } catch(Exception e) {
      System.out.println("exception: " + e.getMessage() + "\n");
    }
  }
};
