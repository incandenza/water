import java.net.*;

public class IPAddress {
  public static void main(String args[]) {
    try {
      InetAddress a = InetAddress.getByName("skavoovie");
      System.out.println("address is " + a);
      a = InetAddress.getLocalHost();
      System.out.println("localhost is " + a);
    } catch(Exception e) {
      System.out.println("got an exception: " + e);
      e.printStackTrace();
    }
  }
}
