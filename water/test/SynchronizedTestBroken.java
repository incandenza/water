public class SynchronizedTestBroken
  implements Runnable
{
  public static int count = 0;

  public void run()
    {
      try {
        count++;

        synchronized(this) {
          notifyAll();
        }
      } catch (Exception e) {
        System.out.println("FAILED: receiver: " + e);
        System.exit(1);
      }
    }
  public static void main(String args[])
    {
      try {
        SynchronizedTestBroken tester = new SynchronizedTestBroken();
        Thread tester_thread = new Thread(tester);

        synchronized(tester) {
          tester_thread.start();
          tester.wait();
        }

        if (0 == count)
          throw new Exception("Thread did not run.");

        tester_thread.join();
          
        System.out.println("PASSED: count="+count);
      } catch (Exception e) {
        System.out.println("FAILED: " + e);
        System.exit(1);
      }
    }
}

