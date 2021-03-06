import javax.swing.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;

public class gui extends JFrame 
{	
	JTextField text = new JTextField(20);
	
	JTextField who = new JTextField("Type address",15);
	JTextField port = new JTextField("Port",3);
	/*JTextField text3 = new JTextField(3);
	JTextField text4 = new JTextField(3);*/

	JList list = new JList();
	
	JTextArea textArea = new JTextArea(20,30);
	//JLabel last_msg = new JLabel();
	Container main_pane = null;
	/*private Timer timer;
	static clientThread thread;
	static ipLookupTable lookthread;
	static msg rec;*/
	
	JButton call = new JButton("Call");
	
	public UAC client = null;
	
	
	public gui() 
	{
		super("VoIP");
		//timer = new Timer(700,this);
		
		
        	setSize(600, 500);
		setVisible(true);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		
		Color c = Color.DARK_GRAY;
		

		JScrollPane scrollPane = new JScrollPane(textArea);

		
		
		JPanel pane1 = new JPanel();
		pane1.setLayout(new BorderLayout());
		pane1.add(scrollPane,BorderLayout.CENTER);
		pane1.setBackground(c);
		//pane1.add(last_msg,BorderLayout.NORTH);
		
		JPanel pane3 = new JPanel();
		
		
		pane3.add(who);
		pane3.add(port,BorderLayout.SOUTH);
		pane3.setBackground(c);
		//pane3.add(text4);
		
		JPanel pane4 = new JPanel();
		pane4.setLayout(new BorderLayout());
		pane4.add(list,BorderLayout.SOUTH);
		pane4.add(pane3,BorderLayout.NORTH);
		pane4.setBackground(c);
		
		JPanel pane2 = new JPanel();
		pane2.setLayout(new BorderLayout());
		pane2.add(call,BorderLayout.SOUTH);
		
		pane2.add(pane4,BorderLayout.CENTER);
		pane2.setBackground(c);
		
		JPanel main_panel = new JPanel();
		main_panel.setLayout(new BorderLayout());
		main_panel.add(pane1,BorderLayout.WEST);
		main_panel.add(pane2,BorderLayout.EAST);
		
		main_pane = getContentPane();
		getContentPane().setLayout( new BorderLayout());
		main_panel.setBackground(c);
		main_pane.add(main_panel,BorderLayout.CENTER);
		
		call.addActionListener(new ActionListener()
		{
			public void actionPerformed(ActionEvent a)
			{
				
				makeCall();
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
			int theport = 0;//(new Integer(port.getText())).intValue();
			textArea.append("Calling "+who.getText()+":"+theport+"\n");
			client.call(who.getText(),theport);
		}
	}
	public void addMsg(String msg)
	{
		textArea.append(msg+"\n");
	}
	public void setClient(UAC client)
	{
		this.client = client;
	}
}
	


