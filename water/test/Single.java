final class Single {
    private Single () {
      System.out.println("constructor called.");
    }

  public void doSomething() {
    System.out.println("doing something.");
  }

    public static final Single instance = new Single();
}

class Test {
  public static void main(String args[]) {
    Single s = Single.instance;
    s.doSomething();
  }
}
