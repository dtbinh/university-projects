import java.io.*;
import java.util.*;
import java.lang.*;
public class grail{
	private FileReader dfa;
	private String fileName;
	private LinkedList lang,allState;
	
	public grail(String fileName) {
		this.fileName = fileName;
	}
	public LinkedList getLangList() {
		return lang;
	}
	public LinkedList getStateList() {
		return allState;
	}
	public HashMap getDfaTransitionList(){
		HashMap hash = null;
		
		try{
			hash = new HashMap();
			dfa = new FileReader(fileName);
			char chr;
			state transition = null;;
			do {
				transition = readStates();
				if (transition != null) {
					hash.put(new String(""+transition.getFirst()+transition.getChar()),transition);
				}
				
			} while (transition != null);
			dfa.close();
		}catch (Exception e)
		{
			System.out.println("Error reading "+fileName+" while creating Transition List");
			System.exit(0);
		}
		
		return hash;
	}
	
	public LinkedList[] getDfaAdjacencyList() {
		LinkedList[] list = new LinkedList[allState.size()];
		try{
			dfa = new FileReader(fileName);
			char chr;
			int i;
			state transition = null;
			do {
				transition = readStates();
				if (transition != null) {
					i = allState.indexOf(new String(""+transition.getFirst()));
					if (list[i] == null) {
						LinkedList temp = new LinkedList();
						temp.add(transition);
						list[i] = temp;
					}else {
						list[i].add(transition);
					}
				}
				
			} while (transition != null);
			dfa.close();
		}catch (Exception e) {
//			e.printStackTrace();
			System.out.println("Error reading "+fileName+" while creating Adjacency List");
			System.exit(0);
		}
		
		return list;
	}
	
	public int[][] getDfaTransitionMatrix() {
		try {
			dfa = new FileReader(fileName);
			allState = new LinkedList();
			lang = new LinkedList();
			state transition = null;
			do {
				transition = readStates();
				if (transition != null) {
					if ( ! allState.contains(new String(""+transition.getFirst()))) {
						allState.add(new String(""+transition.getFirst()));
					}
					if ( ! lang.contains(new String(""+transition.getChar()))) {
						lang.add(new String(""+transition.getChar()));
					}
				}
			}while (transition != null);
			dfa.close();
			dfa = new FileReader(fileName);
			int[][] matrix = new int[allState.size()][lang.size()];
			
			do {
				transition = readStates();
				if (transition != null) {
					matrix[allState.indexOf(new String(""+transition.getFirst()))][lang.indexOf(new String(""+transition.getChar()))] = transition.getSecond();
				}
			}while (transition != null);
			dfa.close();
			return matrix;
		}catch (Exception e) {
			e.printStackTrace();
			System.out.println("Error reading "+fileName+" while creating Transition Matrix");
			System.exit(0);
			return null;
		}
	}
	
	private state readStates() {
		int status = 0;
		state transition = null;
		try {
			status = dfa.read();
			while ((status == 32) || (status == 10)) {
				status = dfa.read();
				//Removing whitespace
			}
			
			if (status == -1)
				return null;
			else{
				if (status == 83) {
					status = dfa.read();
					if (status == 84) {
						status = dfa.read();
						if (status == 65) {
							status = dfa.read();
							if (status == 82) {
								status = dfa.read();
								if (status == 84) {
									status = dfa.read();
									if (status == 45) {
										status = dfa.read();
										status = status - 48;
										System.out.println("START STATE "+status);
										transition = new state(-1,status,(char)0);
										
									}else {
										return null;
									}
								} else {
									return null;
								}
							} else {
								return null;
							}
						} else {
							return null;
						}
					} else {
						return null;
					}
				} else {
					if ((status < 48) || (status > 57)) {
						return null;
					}
					int start, end;
					char alph;
					
					start = status - 48;
					status = dfa.read();
					while ((status >=48) && (status <= 57)) {
						start = start*10 + status-48;
						status = dfa.read();
					}
					if (status == 45) {
						status = dfa.read();
						if (status == 69) {
							status = dfa.read();
							if (status == 78) {
								status = dfa.read();
								if (status == 68) {
									System.out.println("State "+start+" is a final state");
									transition = new state(start,-1,(char)0);
								}else {
									return null;
								}
							}else {
								return null;
							}
						}else {
							return null;
						}
					}else {
						while (status == 32) {
							status = dfa.read();
						}
						alph = (char)status;
						status = dfa.read();
						while (status == 32) {
							status = dfa.read();
						}
						end = status - 48;
						status = dfa.read();
						while ((status >=48) && (status <= 57)) {
							end = end*10 + status-48;
							status = dfa.read();
						}
						System.out.println("Transition from "+start+" to "+end+", on "+alph);
						transition = new state(start,end,alph);
					}
				}
				return transition;
			}
		}catch (Exception e) {}
		return null;
	}
}
