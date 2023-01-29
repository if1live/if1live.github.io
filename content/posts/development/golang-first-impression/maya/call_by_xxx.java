class CallByXXX {
	public CallByXXX() {
		this.a = 0;
	}
	public int a;

	public static void callByReference(CallByXXX obj) {
		obj.a += 1;
		System.out.println(String.format("in function : %d", obj.a));
	}

	public static void main(String [] args) {
		CallByXXX obj = new CallByXXX();

		System.out.println(String.format("before call by reference : %d", obj.a));
		callByReference(obj);
		System.out.println(String.format("after call by reference : %d", obj.a));
	}
}
