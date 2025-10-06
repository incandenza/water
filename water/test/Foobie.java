public class Foobie {
  public Foobie() {
    System.out.println("constructor called.");
  }

  public static void main(String[] args) {
    Foobie[] foobie_array = new Foobie[25];

    foobie_array[0] = new Foobie();

    System.out.println("bye...\n");
  }
}
