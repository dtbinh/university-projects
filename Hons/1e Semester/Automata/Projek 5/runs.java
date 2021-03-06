import java.io.*;
import java.util.*;

public class runs{
	
	ArrayList list;
	int[] r = new int[6];
	double n;
	
	private static final double[][] a = 
	{{4529.4, 9044.9, 13568.0, 18091.0, 22615.0, 27892.0},
	{9044.9, 18097.0, 27139.0, 36184.0, 45234.0, 55789.0},
	{13568.0, 27139.0, 40721.0, 54281.0, 67852.0, 83685.0},
	{18091.0, 36184.0, 54281.0, 72414.0, 90470.0, 111580.0},
	{22615.0, 45234.0, 67852.0, 90470.0, 113262.0, 139476.0},
	{27892.0, 55789.0, 83685.0, 111580.0, 139476.0, 172860.0}};
	
	private static final double[] b = {1/6, 5/24, 11/120, 19/720, 29/5040, 1/840};
	
	public runs(){
	}
	
	
	public void parse(){
	
		list = new ArrayList();
		String filename = "int.txt";
		
		
		try{
			FileInputStream f = new FileInputStream(filename);
			Reader input = new BufferedReader(new InputStreamReader(f));
			StreamTokenizer stream = new StreamTokenizer(input);
			
			
			//read the file to the end
			while (stream.nextToken() != StreamTokenizer.TT_EOF) {
				
				//is a number being read
				if (stream.ttype == StreamTokenizer.TT_NUMBER) {
					list.add(new Double(stream.nval));
				}
			}
			//close the file
			f.close();
		}catch (Exception e) {e.printStackTrace();}
	}
	
	public void run(){
		int current = 0;
		
		double temp,prev = 0.0;
		while (!list.isEmpty()){
			temp = ((Double)list.remove(0)).doubleValue();
			n++;
			if (temp >= prev){
				current++;
				prev = temp;
			}else{
				prev = 0.0;
				if (current >= 6){
					r[5] ++;
				}else{
					r[current-1]++;
				}
				current = 0;
			}
		}
		System.out.println("Set of r is");
		for (int i=0;i<6;i++){
			System.out.println("R["+(i+1)+"] = "+r[i]);
		}
	}
	
	public void calc(){
		double R = 0.0;
		for (int i=0;i < 6;i++){
			for (int j=0;j < 6;j++){
				R = R + (a[i][j] * (r[i] - n*b[i]) * (r[j]- n*b[j]));
			}
		}
		R = R / n;
		System.out.println("Test statistic R = "+new Double(R).longValue());
	}
	
	public static void main(String[]args){
		
		runs test = new runs();
		
				
		System.out.println("Parsing");
		
		test.parse();
		test.run();
		test.calc();
		
	}
}