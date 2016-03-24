import java.io.Console;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

public class TestRegex {

    public static void main(String[] args){
        Console console = System.console();
        if (console == null) {
            System.err.println("No console.");
            System.exit(1);
        }
        while (true) {

            // Pattern pattern = 
            // Pattern.compile(console.readLine("%nEnter your regex: "));

            Pattern pattern = 
	    	//Pattern.compile("\\{@code[ ]?(.*)\\}", Pattern.DOTALL);
		Pattern.compile("\\{@code[ ]?(((\\\\})?[^\\}]?)*)\\}", Pattern.DOTALL);
	    // match : {@code blaazodk \}  adapok \} amzdkak } 
	    // and get "blaazodk \}  adapok \} amzdkak" as group 1 

            Matcher matcher = 
		pattern.matcher(console.readLine("Enter input string to search: "));

	    String replacement = matcher.replaceAll("$1");
	    System.out.println("Replaced string: "+replacement);

            boolean found = false;
            while (matcher.find()) {
                console.format("I found the text" +
                    " \"%s\" starting at " +
                    "index %d and ending at index %d.%n",
                    matcher.group(1),
                    matcher.start(),
                    matcher.end());
                found = true;
            }

            if(!found){
                console.format("No match found.%n");
            }
        }
    }
}
