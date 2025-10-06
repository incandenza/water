class Test1 {
  static {
    System.out.println("in Test1's <clinit>.");
  }
}

class Test2 {
  public static void main(String args[]) {
    try {
      Class test1_class = Class.forName("Test1");
    } catch(Exception e) {
      System.out.println("got exception: " + e.getMessage());
      e.printStackTrace();
    }
  }
}

class Test3 {
  public static void main(String args[]) {
    Test1 a = null;
  }
}

class Test4 {
  public static void foo(Test1 a) {}
    
  public static void main(String args[]) {
    foo(null);
  }
}
