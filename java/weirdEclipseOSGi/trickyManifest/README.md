# Embedded/External dependencies in Eclipse OSGi jar

This manifest contains the lines:

Bundle-ClassPath: lib/algos-0.0.1-SNAPSHOT-jar-with-dependencies.jar,
 external:$LIB_LPSOLVEJ_ADFG$/lpsolve55j.jar,
 .

which enables to references other jar dependencies to load in the class path.
Note that this not work by simply add this jar paths to the plain java 
"ClassPath" manifest attribute. This system is dedicated to Eclipse/OSGi.

First line refers to an embedded jar in the lib folder. Do not forget to
reference them in the build.properties file.

Second line refers to an external jar, whose path is resolved thanks to
the *Eclipse* variable LIB_LPSOLVEJ_ADFG. This variable must be defined
with the -D option in the eclipse.ini file; or if you're developping this
manifest plugin in eclipse, this variable must be set in the VM arguments
of the run configuration.