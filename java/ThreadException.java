import java.lang.*;

public class ThreadException {

    class TestThread implements Runnable {

    public void run() {
	throw new RuntimeException();
    }

    }

    public static void main(String[] args) {

	Thread t = new Thread(new ThreadException().new TestThread());
	t.setUncaughtExceptionHandler(new Thread.UncaughtExceptionHandler() {

		public void uncaughtException(Thread t, Throwable e) {
		    System.out.println(t + " throws exception: " + e);
		}
	    });
	// this will call run() function
	t.start();
    }
}
