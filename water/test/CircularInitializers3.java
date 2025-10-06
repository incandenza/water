class A {
  static int n1 = 1;
  static int n2 = B.getValue();

  public static int getValue() { return 2; }
}

class B {
  static int n1 = 3;
  static int n2 = A.getValue();

  public static int getValue() { return 4; }
}

class Test {
  public static void main(String args[]) {
    Thread aThread = new AThread();
    Thread bThread = new BThread();
    
    aThread.start();
    bThread.start();
  }
}

class AThread extends Thread {
  public void run() {
    System.out.println("A.n1 is " + A.n1);
    System.out.println("A.n2 is " + A.n2);
  }
}

class BThread extends Thread {
  public void run() {
    System.out.println("B.n1 is " + B.n1);
    System.out.println("B.n2 is " + B.n2);
  }
}
