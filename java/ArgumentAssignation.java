public class ArgumentAssignation {

    private double value;

    public ArgumentAssignation(double value) {
	this.value = value;
    }

    public static ArgumentAssignation swapInv(ArgumentAssignation a) {
	a = new ArgumentAssignation(1. / a.value);
	return a;
    }

    public static void main(String[] arg) {
	ArgumentAssignation a = new ArgumentAssignation(2.);
	swapInv(a).print();
	a.print();
    }


    public void print() {
	System.out.println("Value: "+value);
    }

}
