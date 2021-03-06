import java.net.*;

public class VoIP
{
	static int proxyPort = 5060;
	static int callPort;
	static int listenPort;
	
	
	
	public static void main(String args[])
	{
		UAC client = null;
		UAS server = null;
		//int disp = (new Integer(args[1]).intValue()) * 2;
		String myName = args[0];
		
		
		DatagramSocket outgoing = null,incoming = null, proxy = null;
		try
		{
			outgoing = new DatagramSocket();
			incoming = new DatagramSocket();
			outgoing.setSoTimeout(500);
			//proxy = new DatagramSocket(proxyPort);
		}catch (Exception e){e.printStackTrace();}
		callPort = outgoing.getLocalPort();
		gui main_frame = new gui();
		main_frame.hide();
		client = new UAC(outgoing,proxy,incoming.getLocalPort(),main_frame);
		server = new UAS(incoming,proxy,main_frame);
		client.Register(myName);
		client.start();
		server.start();
		main_frame.setUserAgents(client,server);
		main_frame.show();
		main_frame.addMsg("My info\n========\ncalls on:"+outgoing.getLocalPort()+"\nlistens on:"+incoming.getLocalPort()+"\n");
	}
}