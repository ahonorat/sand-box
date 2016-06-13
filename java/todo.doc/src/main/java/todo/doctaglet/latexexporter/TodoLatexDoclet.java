package todo.doctaglet.latexexporter;

import java.io.FileWriter;
import java.io.IOException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.sun.javadoc.ClassDoc;
import com.sun.javadoc.ConstructorDoc;
import com.sun.javadoc.Doclet;
import com.sun.javadoc.ExecutableMemberDoc;
import com.sun.javadoc.MethodDoc;
import com.sun.javadoc.RootDoc;
import com.sun.javadoc.Tag;

/**
 * Doclet to produce a tex output with
 * a sum-up of all comments in <code>@todo</code>
 * tags. 
 * 
 * @author ahonorat
 *
 */
public class TodoLatexDoclet extends Doclet {


    public static String QUALIFIER_PREFIX = "todo.doctaglet.";

	/**
	 * Standard constructor, do nothing.
	 */
	public TodoLatexDoclet() {	
	}

	/**
	 * This method create a file named <code>todo.tex</code>
	 * with a section titled "Todo". Then it analyses all classes
	 * to append the document with the collected <code>@todo</code>
	 * comments.
	 * 
	 * @param root
	 * @return
	 */
	public static boolean start(RootDoc root) {

		try {
			FileWriter texOutput = null;
			texOutput = new FileWriter("todo.tex");

			texOutput.write("\\section{Todo}\n\n\n");

			ClassDoc[] clsds = root.classes();		
			for (ClassDoc clsd: clsds) {
				processClass(texOutput, clsd);
			}

			texOutput.write("\n\n");
			
			if (texOutput != null) {
				texOutput.close();
			}

		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return false;
		}

		return true;
	}

	/**
	 * For each class, get the todo comment and write it on the file, 
	 * Then it parses the constructors, the methods and the
	 * inner classes.
	 * 
	 * @param fw File to append.
	 * @param doc Document to analyse (a class).
	 * @throws IOException If file writing went wrong.
	 */
	private static void processClass(FileWriter fw, ClassDoc doc) throws IOException {
		Tag[] todoTags = doc.tags("todo");
		if (todoTags != null && todoTags.length > 0) {
			String title = shortenAndEscape(doc.qualifiedName());
			fw.write("\\subsection*{"+title+"}\n");
			fw.write(getTodoText(todoTags));
			fw.write("\n\n");
		}
		for (ConstructorDoc md: doc.constructors()) {
			processMember(fw, md);
		}
		for (MethodDoc md: doc.methods()) {
			processMember(fw, md);
		}
		for (ClassDoc cld: doc.innerClasses()) {
			processClass(fw, cld);
		}
	}

	/**
	 * Get the todo comment and write a pretty version on the file.
	 * 
	 * @param fw File to append.
	 * @param md Element to analyse.
	 * @throws IOException If file writing went wrong.
	 */
	private static void processMember(FileWriter fw, ExecutableMemberDoc md) throws IOException {
		Tag[] todoTags = md.tags("todo");
		if (todoTags != null && todoTags.length > 0) {
			String title = shortenAndEscape(md.qualifiedName());
			fw.write("\\subsection*{"+title+md.flatSignature()+"}\n");
			fw.write(getTodoText(todoTags));
			fw.write("\n\n");
		}
	}

	/**
	 * Get the todo printable text by suppressing the style attribute,
	 * and escaping special characters and <code>{@code ...}</code> text.
	 * 
	 * @param todoTags Todo tags to sum up.
	 * @return New string ready for LaTeX printing.
	 */
	private static String getTodoText(Tag[] todoTags) {
		StringBuilder sb = new StringBuilder();
		for (Tag tag: todoTags) {
			String rawText = tag.text().replace("<span class=\"todo\">--&gt;&gt;</span>", "");
			String pureText = escapeForLatex(rawText);
			sb.append(pureText+"\n");
		}
		return sb.toString();
	}

	/**
	 * Escape the _, &amp;, &lt; and &gt; characters, also change
	 * <code>{@code ...\}....}</code> in <code>\texttt{...}</code>.
	 * 
	 * @param text String to escape.
	 * @return New string ready for LaTeX printing.
	 */
	private static String escapeForLatex(String text) {
        Pattern pattern = Pattern.compile("\\{@code[ ]?(((\\\\})?[^\\}]?)*)\\}", Pattern.DOTALL);
        Matcher matcher = pattern.matcher(text);
        String tmp = matcher.replaceAll("\\\\texttt{$1}");
		
		tmp = tmp.replace("_", "\\_");

		tmp = tmp.replace("&amp;", "\\&");
		tmp = tmp.replace("&lt;", "$<$");
		tmp = tmp.replace("&gt;", "$>$");
		return tmp;
	}
	
	/**
	 * Remove qualifier prefix of our project:
	 * and escape the string.
	 * 
	 * @param text String to shorten and escape.
	 * @return New string ready for LaTeX printing.
	 */
	private static String shortenAndEscape(String text) {
	    String shortVersion = text.replace(QUALIFIER_PREFIX, "");
		return escapeForLatex(shortVersion);
	}
	
}


