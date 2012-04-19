import javax.swing.*;
import java.awt.*;
import java.awt.event.*;


public class hw1 extends JFrame{
	JList choice;
	JTextArea left,right;
	public hw1(){
		super("Knapsack");
		setSize(400,400);
		setResizable(false);
		
		String[] data = {"01-Knapsack","Fraqtional Knapsack","Brute Knapsack"};
		choice = new JList(data);
		choice.setSelectedIndex(0);
		
		left = new JTextArea("4 20\n2 3\n2 6\n6 25\n2 80",19,17);
		right = new JTextArea("",19,18);
		
	
		
		JButton b_com = new JButton("Compute");
		
		JPanel pane1 = new JPanel();
		pane1.setLayout(new BorderLayout());
		pane1.add(choice,BorderLayout.NORTH);
		
		
		JPanel pane3 = new JPanel();
		pane3.setLayout(new BorderLayout());
		pane3.add(left,BorderLayout.NORTH);
		
		JPanel pane4 = new JPanel();
		pane4.setLayout(new BorderLayout());
		pane4.add(right,BorderLayout.NORTH);
		pane4.add(b_com,BorderLayout.SOUTH);
		
		JPanel pane2 = new JPanel();
		pane2.setLayout(new BorderLayout());
		pane2.add(pane3,BorderLayout.WEST);
		pane2.add(pane4,BorderLayout.EAST);
		
		Container main = getContentPane();
		getContentPane().setLayout(new BorderLayout());
		main.add(pane1,BorderLayout.NORTH);
		main.add(pane2,BorderLayout.SOUTH);
		
		
		this.show();
		
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		b_com.addActionListener(new ActionListener() {
			
			public void actionPerformed(ActionEvent a) {
				boolean input = true;
				int W = 10;
				while (input){
					String tempstr = JOptionPane.showInputDialog(null,"What is the size of the knapsack","Knapsack size.",JOptionPane.QUESTION_MESSAGE);
					try{
						W = Integer.parseInt(tempstr);
						input = false;
					}catch (Exception e){}
				}
				Item[] items = fill(left);
				
				switch (choice.getSelectedIndex()){
					case 0:
						Knapsack sack1 = new Knapsack(items,W);
						sack1.sort(right);
						break;
					case 1:
						FraqKnapsack sack2 = new FraqKnapsack(items,W);
						sack2.sort();
						sack2.results(right);
						break;
					case 2:
						bruteKnapsack sack3 = new bruteKnapsack(items,W);
						sack3.sort();
						sack3.results(right);
						break;
				}
				
				
			}
		
		});
	}
	
	public Item[] fill(JTextArea text){
		int lines = text.getLineCount();
		Item[] items = new Item[lines];
		String t = text.getText();
		String[] line = t.split("\n");
		int w,v;
		for (int i=0;i<lines;i++){
			String[] t2 = line[i].split(" ");
			w = (Integer.valueOf(t2[0])).intValue();
			v = (Integer.valueOf(t2[1])).intValue();
			items[i] = new Item(w,v);
		}
		return items;
	}
	
	public static void main (String[] args){
		hw1 gui = new hw1();
	}
}