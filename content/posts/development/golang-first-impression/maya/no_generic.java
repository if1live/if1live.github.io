import java.util.ArrayList;

class NoGeneric {
	public void useGeneric() {
		ArrayList<Integer> arr = new ArrayList<Integer>();
		arr.add(1);
		// compile error
		// arr.add("string");

		for(int a : arr) {
			System.out.println(a * 10);
		}
	}

	public void useNoGeneric() {
		ArrayList arr = new ArrayList();
		arr.add(1);
		arr.add("string");

		for(Object obj : arr) {
			if(obj instanceof Integer) {
				int val = ((Integer)obj).intValue();
				System.out.println(val * 10);
			}
		}
	}

	public static void main(String []args) {
		NoGeneric obj = new NoGeneric();
		obj.useGeneric();
		obj.useNoGeneric();
	}
}
