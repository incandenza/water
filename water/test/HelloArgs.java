public class HelloArgs {
  public static void main(String[] args) {
    System.out.println("Hello, world!");
    System.out.println("got " + args.length + " args.");
    for(int i = 0; i < args.length; i++) {
      System.out.println("arg " + i + " is " + args[i]);
    }
  }
}
