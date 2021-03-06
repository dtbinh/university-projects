import java.io.*;
import java.util.*;
import java.net.*;



public class gateway
{
	private static int serv_port = 1050,local,rec;
	static String group = "224.0.0.0";
	public static void main(String args[]) throws Exception
	{
		DatagramSocket r_clients = null,s_clients = null; 
		MulticastSocket servers = null, b_clients = null;
		
		local = Integer.parseInt(args[1]);
		rec = Integer.parseInt(args[0]);
		
		System.out.println("Recieving arp on port "+rec+"...");
		System.out.println("Recieving reply arp on port "+(rec+1)+"...");
		System.out.println("\nEmulating gateway on port "+local+"\nWaiting...");
		
		try
		{
			r_clients = new DatagramSocket(rec);
			
			s_clients = new DatagramSocket(rec+1);
			
			b_clients = new MulticastSocket(local);
			b_clients.joinGroup(InetAddress.getByName(group));
			
			servers = new MulticastSocket(serv_port);
			servers.joinGroup(InetAddress.getByName(group));
			
			servers.setSoTimeout(10);
			s_clients.setSoTimeout(10);
			b_clients.setSoTimeout(10);
			r_clients.setSoTimeout(10);
		}catch (Exception e){System.out.println("error: "+e.toString());System.exit(1);}
                
		byte arp_buf[] = new byte[28];
		DatagramPacket arp_packet = new DatagramPacket(arp_buf,arp_buf.length);
		
		while (true)
		{
			try
			{
				servers.receive(arp_packet);
				System.out.println("\nGot arp from servers");
				
				System.out.println("Sending arp to local computers");
				arp_packet.setAddress(InetAddress.getByName(group));
				arp_packet.setPort(local);
				try
				{
					b_clients.send(arp_packet,(byte)255);
				}catch (Exception e3){}
			}catch (Exception e)
			{
				try
				{
					r_clients.receive(arp_packet);
					System.out.println("\nGot arp from client");
					
					System.out.println("Sending ARP");
					arp_packet.setAddress(InetAddress.getByName(group));
					arp_packet.setPort(serv_port);
					try
					{
						servers.send(arp_packet,(byte)255);
					}catch (Exception e3){}
				}catch (Exception e2)
				{
					try
					{
						s_clients.receive(arp_packet);
						System.out.println("\nGot reply arp from client");
						
						arp_packet.setAddress(InetAddress.getByName(group));
						arp_packet.setPort(2500);
						try
						{
							s_clients.send(arp_packet);
						}catch (Exception e3){}
					}catch (Exception e4)
					{}
				}
			}
		}
	}
}
