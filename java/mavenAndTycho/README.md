This repertory test building standard maven projects as OSGi dependencies for a tycho maven project.

It contains 4 folders:

1) doNotWork/ 
   because depdencies are not explicit in the pom of the tycho project 
   (also, do not forget <pomDependencies>consider<pomDependencies>). The pure maven
   project doesn't have any dependencies.

2) doWork/ 
   actually exactly as 1) but with the above fix.

3) doAlsoNotWork/
   this time the maven project contains dependencies and embed them.
   This project works when compiling with Maven, but causes some
   troubles when importing it into Eclipse.

4) doAlsoWork/
   same as 3) but this time the maven project depdencies are wrapped in a
   separated bundle. Thus the tycho project has two depdencies: one on
   each of the maven project (the user code and its depencies' wrapper).
   This one can be imported into Eclipse.