class A {
  static { System.out.println("A's <clinit> called."); }
  static int n1 = 1;
  static int n2 = B.getValue();

  public static int getValue() { return 2; }
}

class B {
  static { System.out.println("B's <clinit> called."); }
  static int n1 = 3;
  static int n2 = A.getValue();

  public static int getValue() { return 4; }
}

class Test {
  public static void main(String args[]) {
    System.out.println("A.n1 is " + A.n1);
    System.out.println("A.n2 is " + A.n2);
    System.out.println("B.n1 is " + B.n1);
    System.out.println("B.n2 is " + B.n2);
  }
}
