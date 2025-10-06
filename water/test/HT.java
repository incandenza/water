import java.util.Hashtable;

public class HT {
  public static void main(String[] args) {
    Hashtable ht = new Hashtable();
    ht.put("LIST","CRequestLIST");
    ht.put("CAPABILITY","CRequestCAPABILITY");
    ht.put("NOOP","CRequestNOOP");
    
    System.out.println("from hash table " + ht.get("NOOP"));
  }
}
