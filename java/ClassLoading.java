public class ClassLoading {

    // class loader
    private static ClassLoader cl = ClassLoader.getSystemClassLoader();

    static {
	String pathToJar = "cplex.jar";
	URL[] url = {new URL("file:" + pathToJar)};
	cl = URLClassLoader.newInstance(url, cl);
					
					
	Thread.currentThread().setContextClassLoader(cl);
	// useless, depending on the framework

	StringBuilder sb = new StringBuilder();
	sb.append("Loaded classes:\n");
	JarFile jarFile = new JarFile(pathToJar);
	Enumeration<?> e = jarFile.entries();
	while (e.hasMoreElements()) {
	    JarEntry je = (JarEntry) e.nextElement();
	    if (je.isDirectory() || !je.getName().endsWith(".class")) {
		continue;
	    }
	    // -6 because of .class
	    String className = je.getName().substring(0,je.getName().length()-6);
	    className = className.replace('/', '.');
	    Class<?> c = cl.loadClass(className);
	    sb.append(className+"\n");
	} 
	jarFile.close();

	// method with the new class loader must be invoked by reflection
					
    } 

}
