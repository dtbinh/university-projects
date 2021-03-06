import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferStrategy;
import java.util.*;
import javax.swing.*;

public class project3b implements MouseListener{
	
	Frame mainFrame;
	BufferStrategy bufferStrategy;
	Rectangle bounds;
	GraphicsDevice device;
	
	int rows,cols,rowInc,colInc,gx,gy,rx,ry,ix,iy;
	boolean done;
	
	state[][] ca;

	public project3b(GraphicsDevice device,state[][] ca) {
		
		this.ca = ca;
		rows = ca.length;
		cols = ca[0].length;
		this.device = device;

		//initiate the fullscreen graphics
		try {
			GraphicsConfiguration gc = device.getDefaultConfiguration();
			mainFrame = new Frame(gc);
			mainFrame.addMouseListener(this);
			mainFrame.setUndecorated(true);
			mainFrame.setIgnoreRepaint(false);
			device.setFullScreenWindow(mainFrame);

			bounds = mainFrame.getBounds();
			mainFrame.createBufferStrategy(2);
			bufferStrategy = mainFrame.getBufferStrategy();
			
			rowInc = bounds.height / rows;
			colInc = bounds.width / cols;
		} catch (Exception e) {
			System.out.println("Problem entering fullscreen mode");
			System.exit(1);
		}
	}

	//mouseevents
	public void mouseReleased(MouseEvent e){}
	public void mouseExited(MouseEvent e){}
	public void mouseEntered(MouseEvent e){}
	public void mousePressed(MouseEvent e){
	}
	public void mouseClicked(MouseEvent e){
		if (e.getButton() == MouseEvent.BUTTON3) {
			rx = ix;
			ry = iy;
		}else{
			device.setFullScreenWindow(null);
			System.exit(0);
		}
	}

	
	public BufferStrategy getStrategy() {
		return bufferStrategy;
	}
	//draws the grid
	public void drawGrid(Graphics g) {
		g.setColor(Color.black);
		for (int i=0;i<bounds.width;i=i+colInc) {
			g.drawLine(i,0,i,bounds.height);
		}
		for (int i=0;i<bounds.height;i=i+rowInc) {
			g.drawLine(0,i,bounds.width,i);
		}
	}
	//draw a wall cell
	public void set(Graphics g,int row,int col) {
		g.setColor(Color.black);
		g.fillRect(col*colInc,row*rowInc,col+colInc,row+rowInc);
	}
	//draw a empty cell and it's manhattan distance or if it's a goal/initial
	public void clear(Graphics g,int row,int col,state cell) {
		g.setColor(Color.white);
		g.fillRect(col*colInc,row*rowInc,col+colInc,row+rowInc);
		g.setColor(Color.black);
		if (cell.isInitial()) {
			g.drawString("INITIAL",col*colInc+colInc/2,row*rowInc+rowInc/2);
		}else {
			if (cell.isGoal()) {
				g.drawString("GOAL",col*colInc+colInc/2,row*rowInc+rowInc/2);
			}else {
				g.drawString(""+cell.get(),col*colInc+colInc/2,row*rowInc+rowInc/2);
			}
		}
	}
	//compute the manhattan distance to the neighbours
	public int[] neighbours(int drow, int dcol) {
		int[] counter = new int[2];
		counter[0] = 0;
		for (int row = drow-1;row <= drow+1;row++) {
			
			if ((row >= 0) && (row < rows)) {
				
				for (int col=dcol-1;col <= dcol+1;col++) {
					
					if ((col >= 0) && (col < cols) && !(ca[row][col].isWall())) {
						if ((ca[row][col].get() > 0) ){
							counter[0]++;
							if (((ca[row][col].get() < counter[1]) || (counter[1] == 0))){
								counter[1] = ca[row][col].get();
							}
						}
					}
				}
			}
		}
		return counter;
	}
	
	//the first iteration of the algorithm
	public void Moore1() {
		int[][] old = new int[rows][cols];
		for (int row = 0;row < rows;row++) {
			for (int col=0;col < cols;col++) {
				old[row][col] = ca[row][col].get();
				if (ca[row][col].isGoal()) {
					gy = row;
					gx = col;
				}
				if (ca[row][col].isInitial()) {
					ry = row;
					rx = col;
					iy = row;
					ix = col;
				}
			}
		}
		for (int row = 0;row < rows;row++) {
			for (int col=0;col < cols;col++) {
				
				if (old[row][col] == 0) {
					for (int i=row-1;i<=row+1;i++) {
						if ((i >= 0) && (i < rows)) {
							for (int j=col-1;j<=col+1;j++) {
								if ((j >= 0) && (j < cols)) {
									if ((old[i][j] > 0) && !(ca[row][col].isWall())) {
										ca[row][col].set(1);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//when the CA get's flooded
	public void Moore2() {
		int[] counter;
		int[][] old = new int[rows][cols];
		for (int row = 0;row < rows;row++) {
			for (int col=0;col < cols;col++) {
				old[row][col] = ca[row][col].get();
			}
		}
		if (ca[ry][rx].get() == 0) {
			for (int row = 0;row < rows;row++) {
				for (int col=0;col < cols;col++) {
					if (!ca[row][col].isWall()) {
						counter = neighbours(row,col);
						
						if ((counter[0] >= 1) && (ca[row][col].get() == 0)) {
						
							old[row][col] = counter[1]+1;
							
							if ((row == ry) && (col == rx)) {
								done = true;
								break;
							}
							
						}
					}
				}
			}
		}
		for (int row = 0;row < rows;row++) {
			for (int col=0;col < cols;col++) {
				ca[row][col].set(old[row][col]);
			}
		}
	}
	
	//work out the correct neighbour for the robot
	public int[] robotNeighbours(int drow, int dcol) {
		int[] counter = new int[3];
		counter[2] = 0;
		
		for (int row = drow-1;row <= drow+1;row++) {
			if ((row >= 0) && (row < rows) && (row != drow)) {
				
				
				if ((ca[row][dcol].get() > 0) && !(ca[row][dcol].isWall()) ){
				
					if (ca[row][dcol].isGoal()) {
						counter[0] = row;
						counter[1] = dcol;
						counter[2] = -1;
					}
					if ((counter[2] == 0)) {
						counter[0] = row;
						counter[1] = dcol;
						counter[2] = ca[row][dcol].get();
					}else {
						if (ca[row][dcol].get() < counter[2]) {
							counter[0] = row;
							counter[1] = dcol;
							counter[2] = ca[row][dcol].get();
						}
					}
				}
			}
		}
		
		for (int col=dcol-1;col <= dcol+1;col++) {
		
			if ((col >= 0) && (col < cols) && (col != dcol)) {	
		
				if ((ca[drow][col].get() > 0) && !(ca[drow][col].isWall()) ){
					if (ca[drow][col].isGoal()) {
						counter[0] = drow;
						counter[1] = col;
						counter[2] = -1;
					}
					if ((counter[2] == 0)) {
						counter[0] = drow;
						counter[1] = col;
						counter[2] = ca[drow][col].get();
					}else {
						if (ca[drow][col].get() < counter[2]) {
							counter[0] = drow;
							counter[1] = col;
							counter[2] = ca[drow][col].get();
						}
					}
				}
				
			}
		}
		return counter;
	}
	//this moves the robot
	public void walkTheRobot(Graphics g) {
		if ((rx != gx) || (ry != gy)) {
			int[] option= robotNeighbours(ry,rx);
			rx = option[1];
			ry = option[0];
		}
		g.setColor(Color.green);
		g.fillRect(rx*colInc+colInc/3,ry*rowInc+rowInc/3,rx+colInc/2,ry+rowInc/2);
	}
		
	public static void main (String[] args){
	
		
		state[][] ca = input.parse("defs.txt");
		int rows = ca.length;
		int cols = ca[0].length;
		boolean init = true;
		
		System.out.println("Running system");
		//info for the user
		JOptionPane.showMessageDialog(null,"To exit left-click on the screen.\nTo reset the robot's position right-click on the screen","System is running.",JOptionPane.INFORMATION_MESSAGE);
		
		GraphicsEnvironment env = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice device = env.getDefaultScreenDevice();
		project3b gui = new project3b(device,ca);
		Graphics g;
		
		//make sure there's no null states
		for (int row=0; row < rows;row++) {
			for (int col=0; col < cols;col++) {
				if (ca[row][col] == null) {
					ca[row][col] = new state();
				}
			}
		}
		
		while (true) {
			g = gui.getStrategy().getDrawGraphics();
			
			//get ready to draw the system
			for (int row=0; row < rows;row++) {
				for (int col=0; col < cols;col++) {
					if (ca[row][col].isWall()) {
						gui.set(g,row,col);
					}else {
						gui.clear(g,row,col,ca[row][col]);
					}
				}
			}
			//where we at?
			if (init) {
				gui.Moore1();
				init = false;
			}else {
				if (!gui.done) {
					gui.Moore2();
				}else {
					gui.walkTheRobot(g);
				}
			}
			gui.drawGrid(g);
			gui.getStrategy().show();
			g.dispose();
			try {
				Thread.sleep(100);
			}catch(Exception e){}
		}
	}
}