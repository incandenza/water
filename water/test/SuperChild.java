public class SuperChild extends SuperParent {
  public static void foobie() {
    super.foobie();
    System.out.println("bletch");
  }

  public static void main(String[] args) {
    foobie();
  }
};
