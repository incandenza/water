public class JNIClass {
  public static void main(String[] args) {
      System.loadLibrary("jniclass");

      try {
	  foo1();
      } catch(Exception e) {
	  System.err.println("got an exception.");
	  e.printStackTrace();
      }
  }

  public static native void foo1() throws Exception;
  public static void foo2() throws Exception {
      System.err.println("hello from foo2().");
      throw new Exception("you suck");
  }
};
