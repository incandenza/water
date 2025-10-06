import java.util.Properties;

public class ShowProperties {
  public static void main(String[] args) {
    Properties p = System.getProperties();

    p.list(System.out);
  }
}
