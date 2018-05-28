class HelloWorld
{
public native void displayHelloWorld();
static {
    System.out.println( System.getProperty("java.library.path"));
    System.loadLibrary("hello");
}

public static void main(String[] args)
{
    new HelloWorld().displayHelloWorld();
}
}
