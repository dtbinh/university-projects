import java.net.*;
import java.io.*;
import javax.swing.*;

public class UAC extends Thread implements Runnable
{
	static final int proxyPort = 5060;
	private int callPort;
	private int listenPort;
	private int remotePort = 0,remoteVoice;
	static final int max_msg = 1024;
	private DatagramSocket outgoing,proxy;
	private gui VoIp;
	private String proxyA = "",me,remoteAddr;
	private boolean running,docall;
	private int port;
	private String who;
	private ServerSocket waitSock = null;
	private microphone phone = null;
	private speaker speak = null;
	
	public UAC(DatagramSocket outgoing,DatagramSocket proxy,int listenPort,gui VoIp)
	{
		callPort = outgoing.getLocalPort();
		this.listenPort = listenPort;
		this.outgoing = outgoing;
		this.proxy = proxy;
		this.VoIp = VoIp;
		docall = false;
		running = true;
		boolean getport = true;
		while (getport)
		{
			try{
				waitSock = new ServerSocket((new Long(Math.round((Math.random())*10000)+1024)).intValue());
				getport = false;
			}catch(Exception e){}
		}
		System.out.println("[UAC]Voice port = "+waitSock.getLocalPort());
	}
	public void run()
	{
		
		while(running)
		{
			if (docall)
			{
				makeCall();
			}
		}
	}
	
	public void Register(String name)
	{
		proxyA = JOptionPane.showInputDialog("Proxy Address:"); 
		if ((proxyA.compareTo("") == 0) | (proxyA.compareTo("localhost") == 0))
		{
			try
			{
				proxyA = (InetAddress.getLocalHost()).getHostName();
			}catch (Exception e){}
		}
		me = name;
		try
		{
			
			String addr = (InetAddress.getLocalHost()).getHostName();
			String body = "empty";
			//String body = "Name: "+name+",IP: "+(InetAddress.getLocalHost()).getHostName()+",Port: "+listenPort;
			String to_from =name+" <sip:"+name+"@"+addr;
			String header = SIP.Header(to_from+">",to_from+":port="+listenPort+">","proxy: "+proxyA);
			DatagramPacket msg =SIP.createRequest("REGISTER",proxyA,proxyPort,header,body);
			outgoing.send(msg);
		}catch (Exception e){e.printStackTrace();}
	}
	
	public void call(String who,int port)
	{
		this.who = who;
		this.port = port;
		docall = true;
	}
	
	public void makeCall()
	{
		VoIp.addMsg("Making call to "+who);
		//send invite
		String addr = null;
		try
		{
			addr = (InetAddress.getLocalHost()).getHostName();
		}catch (Exception e){}
		String to = who+" <sip: "+who+"@"+proxyA+">";
		String from = me+" <sip:"+me+"@"+addr+">";
		String msgheader = SIP.Header(to,from,"proxy: "+proxyA);
		DatagramPacket msg = SIP.createRequest("INVITE",proxyA,proxyPort,msgheader,"");
		DatagramPacket in_msg = null;
		
		byte buf[] = new byte[max_msg];
		try
		{
			outgoing.send(msg);
			in_msg = new DatagramPacket(buf,buf.length);
			outgoing.receive(in_msg);
			String header[] = SIP.stripResponse(in_msg);
			//if 100, proceed
			if ((header[1].compareTo("100")) == 0)
			{
				VoIp.addMsg("[UAC]Received TRYING = "+header[1]+" from proxy, protocol "+header[0]);
				
				outgoing.receive(in_msg);
				header = SIP.stripResponse(in_msg);
				//if 180, proceed
				if ((header[1].compareTo("180")) == 0)
				{
					//ring a bit
					VoIp.addMsg("[UAC]Received Ringing = "+header[1]+" from proxy, protocol"+header[0]);
					/*for (int i=0;i<100;i++)
					{
						VoIp.setLocation(i%5,i%5);
					}*/
					//while 182, wait for OK
					boolean wait = true;
					//if 200
					while (wait)
					{
						outgoing.receive(in_msg);
						header = SIP.stripResponse(in_msg);
						if ((header[1].compareTo("200")) == 0)
						{
							VoIp.addMsg("[UAC]Received OK = "+header[1]+" from proxy, protocol"+header[0]);
							remoteVoice = SIP.getPortResponse(in_msg);
							System.out.println("[UAC]Remote voice port = "+remoteVoice);
							//send ACK
							String body = SIP.stripResponseHeader(in_msg);
							String a[] = body.split(",");
							String theHoff[] = a[0].split("FROM:");
							String theHoff2[] = a[1].split("PORT:");
							remotePort = (new Integer(theHoff2[1])).intValue();
							remoteAddr = theHoff[1];
							VoIp.addMsg("[UAC]Sending ACK to "+theHoff[1]+" port:"+remotePort);
							msg = SIP.createRequest("ACK",remoteAddr,remotePort,"","port: "+waitSock.getLocalPort());
							outgoing.send(msg);
							wait = false;
							//return who;
							voiceConnect();
						}else
						{
							if ((header[1].compareTo("182")) == 0)
							{
								System.out.println("[UAC]Received Queud = "+header[1]+" from proxy, protocol"+header[0]);
							}
						}
					}
					
				}else
				{
					VoIp.addMsg("Garbage data received");
				}
			}else
			{
				System.out.println("Garbage data received");
			}
		}catch (Exception e){}
		
		//return null;
		
			//if 180, proceed
				//wait for message
				
				//wait for messages
				
				
		//else 302
			//send ACK
			//do call to new address
		docall = false;
	}
	
	public String disconnect(String con)
	{
		//send BYE
		String addr = null;
		try
		{
			addr = (InetAddress.getLocalHost()).getHostName();
		}catch (Exception e){}
		String to = con+" <sip: "+con+"@"+remoteAddr+">";
		String from = me+" <sip:"+me+"@"+addr+">";
		String msgheader = SIP.Header(to,from,"proxy: "+proxyA);
		DatagramPacket msg = SIP.createRequest("BYE",remoteAddr,remotePort,msgheader,"");
		try
		{
			VoIp.addMsg("[UAC]Sending BYE to "+remoteAddr+":"+remotePort);
			outgoing.send(msg);
		}catch (Exception e){}
		
		//wait for message
		//if 200, disconected
		byte buf[] = new byte[max_msg];
		DatagramPacket in_msg = new DatagramPacket(buf,buf.length);
		try
		{
			outgoing.receive(in_msg);
		}catch (Exception e){}
		String header[] = SIP.stripResponse(in_msg);
		//if 100, proceed
		if ((header[1].compareTo("200")) != 0)
		{
			VoIp.addMsg("[UAC]Error disconnecting");
		}else
		{
			VoIp.addMsg("[UAC]OK = 200 received");
			VoIp.addMsg("[UAC]Disconnected from "+con);
		}
		docall = false;
		return null;
	}
	public void stopThread()
	{
		running = false;
	}
	public void voiceConnect()
	{
		if (VoIp.audioEnabled())
		{
			System.out.println("[UAC]Connecting voice to "+remoteAddr);
			try
			{
				Socket talk = new Socket(remoteAddr,remoteVoice);
				speak = new speaker(talk);
				speak.start();
				while (speak.mustWait()){}
				Socket talk2 = waitSock.accept();
				phone = new microphone(talk2);
				phone.start();
			}catch (Exception e){e.printStackTrace();}
		}
	}
}
