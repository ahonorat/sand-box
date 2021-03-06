static {
    String env = "LIB_PARTITION_ADFG";
    String value = System.getenv(env);
    if (value != null) {
	//System.out.format("%s=%s%n", env, value);
    } else {
	throw new Error(env + " is not assigned.");
    }
    System.load(value + "/libpartition.so");  // reference to scotch wrapper 
}

/**
 * Adds the specified path to the java library path
 *
 * @param pathToAdd the path to add
 * @throws Exception
 */
public static void addLibraryPath(String pathToAdd) throws Exception{
    final Field usrPathsField = ClassLoader.class.getDeclaredField("usr_paths");
    usrPathsField.setAccessible(true);

    //get array of paths
    final String[] paths = (String[])usrPathsField.get(null);

    //check if the path to add is already present
    for(String path : paths) {
	if(path.equals(pathToAdd)) {
	    return;
	}
    }

    //add the new path
    final String[] newPaths = Arrays.copyOf(paths, paths.length + 1);
    newPaths[newPaths.length-1] = pathToAdd;
    usrPathsField.set(null, newPaths);
}
