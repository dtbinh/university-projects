public class state{
	int col,x,y,num;
	boolean accept;
	state[] trans;
	
	public state(int num){
		trans = new state[2];
		col = 3;
		accept = false;
		this.num = num;
	}
	
	public void setColor(int col){
		this.col = col;
	}
	
	public void setTrans(int chr, state target){
		trans[chr] = target;
	}
	
	public void setAccept(){
		accept = true;
	}
	
	public int getColor(){
		return col;
	}
	public state getTrans(int chr){
		return trans[chr];
	}
	public boolean getAccept(){
		return accept;
	}
}