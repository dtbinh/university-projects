import java.util.*;
import javax.swing.*;

import java.awt.*;
import java.awt.event.*;

public class dfa extends JFrame{
	
	JTextField input = new JTextField(20);
	JButton test1 = new JButton("Transition Matrix");
	JButton test2 = new JButton("Adjacency List");
	JButton test3 = new JButton("Transition List");
	JTextArea output = new JTextArea(15,20);
	
	int current, start;
	HashMap hash = null;
	int[][] array;
	LinkedList lang, allState;
	LinkedList[] list = null;
	
	public dfa() {
		super("Dfa");
		setSize(400,300);
		setLocation(400,350);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		JPanel pane1 = new JPanel();
		pane1.setLayout(new BorderLayout());
		pane1.add(input,BorderLayout.NORTH);
		
		JPanel pane0 = new JPanel();
		pane0.setLayout(new BorderLayout());
		pane1.add(pane0,BorderLayout.SOUTH);
		
		pane0.add(test1,BorderLayout.WEST);
		pane0.add(test2,BorderLayout.CENTER);
		pane0.add(test3,BorderLayout.EAST);
		
		JPanel pane2 = new JPanel();
		pane2.setLayout(new BorderLayout());
		pane2.add(output,BorderLayout.NORTH);
		
		Container main = getContentPane();
		getContentPane().setLayout(new BorderLayout());
		main.add(pane1,BorderLayout.NORTH);
		main.add(pane2,BorderLayout.SOUTH);
		
		test1.setSize(20,20);
		test2.setSize(20,20);
		test3.setSize(20,20);
		
		test1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent a) {
				output.setText("");
				output.append("Testing "+input.getText()+" on dfa represented with transition matrix\n");
				output.append("Entering state "+start+"\n");
				current = start;
				int length = input.getText().length();
				int index = 0;
				int i,j;
				char chr;
				int transition = -1;
				
				while (index != length) {
					chr = input.getText().charAt(index);
					
					i = allState.indexOf(new String(""+current));
					j = lang.indexOf(new String(""+chr));
					if ((i == -1) || (j == -1)) {
						output.append("Input doesn't satisfy DFA\n");
						break;
					}
					transition = array[i][j];
					output.append("Transition from state "+current+" to state "+transition+"\n");
					current = transition;
					index++;
				}
				i = allState.indexOf(new String(""+current));
				j = lang.indexOf(new String(""+(char)0));
				
				if ((i == -1) || (j == -1)) {
					output.append("Input doesn't satisfy DFA\n");
				}else {
					if ((index == length) && (array[i][j] == -1)){
						output.append("Final state reached. DFA satisfied\n");
					}else {
						output.append("Input doesn't satisfy DFA\n");
					}
				}
			}
		});
		
		test2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent a) {
				output.setText("");
				output.append("Testing "+input.getText()+" on dfa represented with adjacency list\n");
				output.append("Entering state "+start+"\n");
				current = start;
				int length = input.getText().length();
				int index = 0,i,j;
				char chr;
				Object[] transitions;
				while (index != length) {
					chr = input.getText().charAt(index);
					i = allState.indexOf(new String(""+current));
					//transition = (state)hash.get(new String(""+current+chr));
					transitions = list[i].toArray();
					j = 0;
					while (j != transitions.length) {
						if ( ((state)transitions[j]).getChar() == chr) {
							break;
						}
						j++;
					}
					if (j == transitions.length) {
						output.append("Input doesn't satisfy DFA\n");
						break;
					} else {
						output.append("Transition from state "+current+" to state "+((state)transitions[j]).getSecond()+"\n");
						current = ((state)transitions[j]).getSecond();
					}
					index++;
				}
				i = allState.indexOf(new String(""+current));
				transitions = list[i].toArray();
				j = 0;
				while (j != transitions.length) {
					if ( ((state)transitions[j]).getChar() == (char)0) {
						break;
					}
					j++;
				}
				
				if ((index == length) && (j != transitions.length)){
					output.append("Final state reached. DFA satisfied\n");
				}else {
					output.append("Input doesn't satisfy DFA\n");
				}
			}
		});
		
		test3.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent a) {
				output.setText("");
				output.append("Testing "+input.getText()+" on dfa represented with transition list\n");
				output.append("Entering state "+start+"\n");
				current = start;
				int length = input.getText().length();
				int index = 0;
				char chr;
				state transition;
				while (index != length) {
					chr = input.getText().charAt(index);
					transition = (state)hash.get(new String(""+current+chr));
					if (transition == null) {
						output.append("Input doesn't satisfy DFA\n");
						break;
					} else {
						output.append("Transition from state "+current+" to state "+transition.getSecond()+"\n");
						current = transition.getSecond();
					}
					index++;
				}
				if ((index == length) && ( ((state)hash.get(new String(""+current+((char)0)))) != null)){
					output.append("Final state reached. DFA satisfied\n");
				}else {
					output.append("Input doesn't satisfy DFA\n");
				}
			}
		});
		
	}
	
	public void setStart(int num) {
		current = num;
		start = num;
	}
	public void setArray(int[][] arrayInput) {
		array = arrayInput;
	}
	public void setList(LinkedList[] listInput) {
		list = listInput;
	}
	public void setHash(HashMap hashInput) {
		hash = hashInput;
		setStart(((state)hash.get(new String(""+-1+(char)0))).getSecond());
	}
	public void setLists(LinkedList langList, LinkedList stateList) {
		lang = langList;
		allState = stateList;
	}
	
	public static void main (String[] args) {
		grail grailInput = new grail(args[0]);
		dfa gui = new dfa();
		
		int[][] array = grailInput.getDfaTransitionMatrix();
		LinkedList[] list = grailInput.getDfaAdjacencyList();
		HashMap hash = grailInput.getDfaTransitionList();
		
		gui.setArray(array);
		gui.setLists(grailInput.getLangList(),grailInput.getStateList());
		gui.setList(list);
		gui.setHash(hash);
		gui.show();
	}
}
