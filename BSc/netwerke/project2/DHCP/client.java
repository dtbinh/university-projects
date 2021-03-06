import java.io.*;
import java.util.*;
import java.net.*;


public class client extends TimerTask
{

	//port the server is listening to	
	private int PORT = 3000;
	public IpPool pool;
	DHCP_MSG msg_in = null;
	DHCP_MSG msg_out = null;
	ObjectOutputStream out = null;
	ObjectInputStream in = null;
	Socket server = null;
	static Timer time = null;
	boolean execute = true;
	
	//the main function
	public static void main(String args[]) throws Exception
	{
		client x = new client();
		//declaration of variables
		
		//try to get a connection to the server
		System.out.println("Waiting for connection to server...");
		while (x.server == null)
		{
			try
			{
				x.server = new Socket(args[0],x.PORT);
			}catch (Exception e){}
		}
		
		//get the streams
		x.out = new ObjectOutputStream(x.server.getOutputStream());   
		x.in = new ObjectInputStream(x.server.getInputStream());   
		
		if ((x.server != null) && (x.server.isConnected()))
		{
			System.out.println("Connected to "+x.server.getInetAddress());
			
			//set a new messages to be sent
			
			x.msg_out = new DHCP_MSG(1);
			
			if(x.msg_out != null)
			{
				x.msg_out.op = 1; //boot request
				x.msg_out.htype = 1;
				x.msg_out.hlen = 6;
				x.msg_out.xid = 1;
				//id++;
				x.msg_out.flags = 0;
				x.msg_out.secs = 0;
				/* next fields are null; 
				 * will be set later at the appropriate times
				 * should maybe set my ip adress here
				 */
				//x.msg_out.chaddr = MacAddr;

				x.msg_out.createOptionsCookie();
				x.msg_out.createDiscovery();
				System.out.println("Node A: Sending DHCPDiscover message");
				x.out.writeObject(x.msg_out);
			}
			else
			{
				System.out.println("Unable to create discovery msg : Exit");
				System.exit(0);
			}
			
			try {
				x.msg_in = (DHCP_MSG)(x.in.readObject());
			}catch (Exception e){System.out.println("error"+e);System.exit(1);}
			
			//try to print the message
			if (x.msg_in != null)
			{
				String type_msg = x.msg_in.getType();
				if(type_msg == "DHCPOffer")
				{
					System.out.println("Node A: Recieving DHCPOffer from DHCP server");
					
					int timeToLease = 0;
					timeToLease = x.msg_in.getLeaseTime();
					String req_ip = x.msg_in.getReqIp();
					
					System.out.println("Node A: The Ip is "+req_ip+" and the lease peroid is "+timeToLease+" seconds");
					
					x.msg_out = x.msg_in;
					x.msg_out.createRequest();
					
					System.out.println("Node A: Accepting offer with DHCPRequest message");
					x.out.writeObject(x.msg_out);
					
					
					//up till here
					//read the reply
				}
				else
				{
					System.out.println("Expecting DHCPOffer but found " + type_msg);
					System.exit(0);
				}
				
				try {
					x.msg_in = (DHCP_MSG)(x.in.readObject());
				}catch (Exception e){System.out.println("error"+e);System.exit(1);}
				
				if(x.msg_in != null)
				{
					type_msg = x.msg_in.getType();
					int timeToLease = 0;
					timeToLease = x.msg_in.getLeaseTime();
					if(type_msg == "DHCPAck")
					{
						System.out.println("Node A: Recieved DHCPAck from DHCP server");
						
						
						time = new Timer();
						time.scheduleAtFixedRate(x,(timeToLease*1000)/2 - 25000,(timeToLease*1000)/2 - 25000); // ie time is set at 5 s
						
					}
				}
				
				
			}else{
				System.out.println("Error: null message recieved");
			}
			
			//close the streams
			//x.in.close();	
			//x.out.close();
		}
	}
	
	public void run()
	{
		if(execute == true)
		{
			System.out.println("Node A: Lease is halfway. Sending DHCPRequest to DHCP SERVER");
			msg_out = msg_in;
			msg_out.createRequest();
		
			try
			{	
				out.writeObject(msg_out);
			}catch (Exception e){}
		
		
			try {
					msg_in = (DHCP_MSG)(in.readObject());
			}catch (Exception e){System.out.println("error"+e);System.exit(1);}
			
			if(msg_in != null)
			{
				String type_msg = msg_in.getType();
				if(type_msg == "DHCPOffer")
				{
					System.out.println("Node A:Recieving DHCPOffer from DHPC Server. Refreshing lease to 60 seconds");
					int timeToLease = 0;
					timeToLease = msg_in.getLeaseTime();
				
					//time = new Timer();
					//Timer time1 = new Timer();
					//time.schedule(this,(timeToLease*1000)/2 - 25000); // ie time is set at 5 s
				}
			}
			else
			{
				System.out.println("Error: null message recieved");
			}
		}
		else
		{
			System.out.println("Node A: Not accepting offer. Declin Ip");
			//close the streams
			try{
				in.close();	
				out.close();
				//close the socket
				System.out.println("Closing socket");
				server.close();
			}
			catch (Exception e){
			e.printStackTrace();
			}
		}
	}
}
