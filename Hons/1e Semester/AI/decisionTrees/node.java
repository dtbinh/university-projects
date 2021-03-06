import java.util.*;
public class node{
	
	boolean[] ilist,cats;
	double entropy,mean;
	int level,index,wrong,right;
	ArrayList children;
	String crit,attr,result;
	node parent = null;
	
	
	public node(boolean[] ilist,boolean[] cats){
		this.ilist = ilist;
		this.cats = cats;
		children = new ArrayList();
		right = 0;
		wrong = 0;
	}
	public node(){
		children = new ArrayList();
		right = 0;
		wrong = 0;
	}
	
	public boolean[] getIlist(){
		return ilist;
	}
	public boolean[] getCats(){
		return cats;
	}
	public double getEntropy(){
		return entropy;
	}
	public String getCrit(){
		return crit;
	}
	public int getCritIndex(){
		return index;
	}
	public String getAttr(){
		return attr;
	}
	public ArrayList getChildren(){
		return children;
	}
	public int getLevel(){
		return level;
	}
	public String getResult(){
		return result;
	}
	public node getParent(){
		return parent;
	}
	
	public void setEntropy(double entropy){
		this.entropy = entropy;
	}
		
	public void setSplitCriteria(String str,int index){
		crit = str;
		this.index = index;
	}
	public void setAttr(String str){
		attr = str;
	}
	public void setLevel(int level){
		this.level = level;
	}
	public void setResult(String res){
		result = res;
	}
	public void setParent(node parent){
		this.parent = parent;
	}
	
	public void addChild(node leaf){
		children.add(leaf);
	}
	public void addMean(double mean){
		this.mean = mean;
	}
}