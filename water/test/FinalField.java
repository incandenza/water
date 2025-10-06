public class FinalField {
  final int a = FinalField.getSomeValue();

  public static void main(String[] args) {
    FinalField f = new FinalField();
  }

  public static int getSomeValue() {
    return 99;
  }
}
