import java.io.*;
import java.util.*;

public class parse {
	
	public static ArrayList run(String filename) {
		ArrayList txt = new ArrayList();
		try {
			FileInputStream f = new FileInputStream(filename);
			Reader input = new BufferedReader(new InputStreamReader(f));
			StreamTokenizer stream = new StreamTokenizer(input);
			stream.whitespaceChars(1,47);
			while (stream.nextToken() != StreamTokenizer.TT_EOF) {
				if (stream.sval != null) {
					txt.add(new String(stream.sval));
				}
			}
			f.close();
		}catch (Exception e) {
			e.printStackTrace();
			System.out.println("Error reading pattern file. Exiting...");
			System.exit(1);
		}
		
		return txt;
	}
	
}