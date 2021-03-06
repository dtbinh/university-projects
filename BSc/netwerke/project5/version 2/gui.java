import javax.swing.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;

public class gui extends JFrame 
{	
	boolean enableAudio = false;
	String con = null;
	JTextField text = new JTextField(20);
	JTextField who = new JTextField("Type address",15);
	
	//JList list = new JList();
	
	JTextArea textArea = new JTextArea(20,30);

	//Container main_pane = null;

	
	JButton call = new JButton("Call");
	JButton disc = new JButton("Disconnect");
	JButton bexit = new JButton("Exit");
	//JButton blog = new JButton("Log");
	JCheckBox blog = new JCheckBox("Log",false);
	JCheckBox aud = new JCheckBox("Audio",false);
	
	JDialog log;
	public UAC client = null;
	public UAS server = null;
	
	
	public gui() 
	{
		super("VoIP");
		//timer = new Timer(700,this);
		this.hide();
		//Container main = getContentPane();
		//main.setLayout(null);
        	setSize(200, 200);
		setLocation(400,350);
		//setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		//color stuff
		Color c = new Color(0x555555);//.darkGray;
		
		call.setBackground(Color.darkGray);
		call.setForeground(Color.WHITE);
		disc.setBackground(Color.darkGray);
		disc.setForeground(Color.WHITE);
		bexit.setBackground(Color.darkGray);
		bexit.setForeground(Color.WHITE);
		blog.setBackground(Color.darkGray);
		blog.setForeground(Color.WHITE);
		aud.setBackground(Color.darkGray);
		aud.setForeground(Color.WHITE);
		//disc.setMaximumSize(new Dimension(12,20));
		//call.setBorder(Color.pink);
		
		disc.setToolTipText("Disconnect current call");
		call.setToolTipText("Make a call");
		blog.setToolTipText("Show debug info");
		bexit.setToolTipText("Exit program");
		aud.setToolTipText("Enable/Disable Audio");
		
		log = new JDialog();
		log.hide();
		log.setSize(600,300);
		log.setLocation(400,10);
		
		JScrollPane scrollPane = new JScrollPane(textArea);
		
		JPanel pane1 = new JPanel();
		pane1.setLayout(new BorderLayout());
		pane1.add(scrollPane,BorderLayout.CENTER);
		pane1.setBackground(c);
		log.setContentPane(pane1);

		

		JPanel pane2 = new JPanel();
		//pane2.setLayout(null);
		//pane2.setBounds(0,0,200,200);
		//pane2.setLayout(null);
		pane2.setLayout(new BorderLayout());
		
		JPanel pane3 = new JPanel();
		JPanel pane4 = new JPanel();
		pane4.setBackground(Color.darkGray);
		//pane3.setLayout(new BorderLayout());
		
		pane3.add(call,BorderLayout.WEST);
		pane3.add(disc,BorderLayout.EAST);
		pane3.add(bexit,BorderLayout.SOUTH);
		pane3.setBackground(c);
		
		pane4.add(blog,BorderLayout.EAST);
		pane4.add(aud,BorderLayout.WEST);
		
		pane2.add(pane3,BorderLayout.CENTER);
		pane2.add(pane4,BorderLayout.SOUTH);
		pane2.add(who,BorderLayout.NORTH);
		
		pane2.setBackground(c);
		
		/*Insets insets = pane2.getInsets();
		call.setBounds(60 + insets.left,60 + insets.right,60,30);
		//who.setLocation(20,20);
		who.setBounds(20 + insets.left,20 + insets.right,120,20);
		blog.setBounds(140 + insets.left,60 + insets.right,60,30);
		
		who.setEnabled(true);*/
		//JPanel main_panel = new JPanel();
		//main_panel.setLayout(new BorderLayout());
		//main_panel.add(pane1,BorderLayout.WEST);
		//main_panel.add(pane2);
		//setContentPane(pane2);
		Container main = getContentPane();
		getContentPane().setLayout(new BorderLayout());
		main.add(pane2,BorderLayout.CENTER);
		//main_pane = getContentPane();
		//getContentPane().setLayout( new BorderLayout());
		//main_panel.setBackground(c);
		//main_pane.add(main_panel);//,BorderLayout.CENTER);
		//call.setSize(20,20);
		
		
		call.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent a)
			{
				
				makeCall();
			}
		});
		disc.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent a)
			{
				
				disconnectCall();
			}
		});
		bexit.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent a)
			{
				
				server.stopThread();
				client.stopThread();
				System.exit(1);
			}
		});
		blog.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent a)
			{
				log.setVisible(!(log.isVisible()));
				
			}
		});
		aud.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent a)
			{
				enableAudio = !enableAudio;
				
			}
		});
		
		
		//timer.start();
		
	}
	public void makeCall()
	{
		if ((who.getText()).compareTo("Type address") == 0)
		{
			textArea.append("Enter valid address\n");
		}else
		{
			if (con == null)
			{
				int theport = 0;//(new Integer(port.getText())).intValue();
				textArea.append("Calling "+who.getText()+"\n");
				client.call(who.getText(),theport);
				con = who.getText();
			}else
			{
				textArea.append("Still connected to "+con+"\n");
			}
		}
	}
	public void disconnectCall()
	{
		if (con != null)
		{
			textArea.append("Disconnecting from "+con+"\n");
			con = client.disconnect(con);
		}else
		{
			textArea.append("Not connected\n");
		}
	}
	public void addMsg(String msg)
	{
		textArea.append(msg+"\n");
	}
	public void setUserAgents(UAC client,UAS server)
	{
		this.client = client;
		this.server = server;
	}
	public boolean audioEnabled()
	{
		return enableAudio;
	}
}
	


