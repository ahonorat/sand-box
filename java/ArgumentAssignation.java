public class ArgumentAssignation {

    private double value;

    public ArgumentAssignation(double value) {
	this.value = value;
    }

    public static void doubleDouble(Double db) {
	db *= 2.0;
    }

    public static ArgumentAssignation swapInv(ArgumentAssignation a) {
	a = new ArgumentAssignation(1. / a.value);
	return a;
    }

    public static void main(String[] arg) {
	ArgumentAssignation a = new ArgumentAssignation(2.);
	a.print();
	swapInv(a).print();

	Double d = new Double(1.0);
	doubleDouble(d);
	System.out.println(d);
    }


    public void print() {
	System.out.println("Value: "+value);
    }

}
