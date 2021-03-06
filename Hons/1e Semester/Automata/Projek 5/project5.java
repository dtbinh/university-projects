import javax.swing.*;
import java.util.*;
import java.io.*;

public class project5{
	int seed, cycle;
	double limit;
	BitSet ca;
	String bitDump = "bit.txt", intDump = "int.txt";
	FileWriter f1,f2;
	
	public project5(int seed, int cycle, double limit){
		this.seed = seed;
		this.cycle = cycle;
		this.limit = limit;
		ca = new BitSet(cycle);
		
		try{
			f1 = new FileWriter(bitDump);
			f2 = new FileWriter(intDump);
		}catch (Exception e){}
		//sit die begin seed om in 'n bitset
		int temp = seed;
		for (int i=0;i<cycle;i++){
			if ((temp % 2) == 1){
				ca.set(i);
			}
			temp = (temp - (temp%2)) / 2;
		}
	}
	//genereer al die nommers
	public void generate(){
		System.out.println("Generating "+limit+" numbers with seed "+seed+" and range "+Math.pow(2,cycle));
		double val;
		
		for (double i=0;i<limit;i++){
			calculate();
			val = bit2int(ca);
			dump(val);
		}
		try{
			f1.close();
			f2.close();
		}catch (Exception e){}
	}
	//make 'n copy van die current seed
	BitSet copy(){
		BitSet cpy = new BitSet(cycle);
		cpy.or(ca);
		return cpy;
	}
	//rotate bits links om
	void rotateleft(BitSet set, int amount){
		boolean temp;
		for (int iter=0;iter < amount;iter++){
			temp = set.get(set.size()-1);
			for (int i=set.size()-1; i > 0;i--){
				set.set(i,set.get(i-1));
			}
			set.set(0,temp);
		}
	}
	//rotate bits regs om
	void rotateright(BitSet set, int amount){
		boolean temp;
		for (int iter=0;iter < amount;iter++){
			temp = set.get(0);
			for (int i=1; i < set.size()-1;i++){
				set.set(i,set.get(i+1));
			}
			set.set(set.size()-1,temp);
		}
	}
	//die hoof funksie om die getal te bereken
	void calculate(){
		BitSet a = copy(),b = copy();
		
		rotateright(a,15);
		rotateleft(b,11);
		a.xor(b);
		ca.xor(a);
		
	}
	//die bitset na double
	double bit2int(BitSet set){
		double count = 0;
		for (int i=0;i<cycle;i++){
			if (set.get(i)){
				count = count + Math.pow(2,i);
			}
		}
		return count;
	}
	//String van die double
	String bitRepr(double val){
		double temp = val;
		String str = "";
		for (int i=0;i<cycle;i++){
			if ((temp % 2) == 1){
				str = "1" + str;
			}else{
				str = "0" + str;
			}
			temp = (temp - (temp%2)) / 2;
		}
		return str;
	}
	//skryf die waarde skyf toe
	void dump(double val){
		try{
			//dump the bits
			f1.write(bitRepr(val)+"\n");
			//dump the doubles
			f2.write(Double.toString(val)+"\n");
		}catch (Exception e){}
	}
	
//==============================================================================================	
	
	public static void main(String[] args){
		String temp;
		boolean input = true;
		int seed = 0,cycle = 0;
		double limit = 0;
		//kry die seed van die user af
		while (input){
			temp = JOptionPane.showInputDialog(null,"What is the seed for the generator?","Seed",JOptionPane.QUESTION_MESSAGE);
			try{
				seed = Integer.parseInt(temp);
				//die seed moet 'n positiewe getal wees
				if (seed >= 0) {
					input = false;
				}
			}catch (Exception e){}
		}
		//kry die aantal bits van die user af
		input = true;
		while (input){
			temp = JOptionPane.showInputDialog(null,"How many bits must the numbers be?","Bits",JOptionPane.QUESTION_MESSAGE);
			try{
				cycle = Integer.parseInt(temp);
				//die seed moet 'n positiewe getal wees
				if (cycle >= 0) {
					input = false;
				}
			}catch (Exception e){}
		}
		//vind uit hoeveel nommers moet gegenereer word
		input = true;
		while (input){
			temp = JOptionPane.showInputDialog(null,"How many numbers must be generated?","Limit",JOptionPane.QUESTION_MESSAGE);
			try{
				limit = Double.parseDouble(temp);
				//die seed moet 'n positiewe getal wees
				if (limit > 0) {
					input = false;
				}
			}catch (Exception e){}
		}
		//begin genereer
		project5 main = new project5(seed,cycle,limit);
		main.generate();
	}
}