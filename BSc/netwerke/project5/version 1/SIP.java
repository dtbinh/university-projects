import java.net.*;
public class SIP
{
	/* Request Methods
	==================
	REGISTER;
	INVITE;
	ACK;
	BYE;
	CANCEL;
	OPTIONS;
	*/
	
	/*Response status
	================
	100 Trying
	180 Ringing
	182 Queued
	200 OK
	302 Moved temporarily
	*/
	
	static final String SIP_VER = "SIP/2.0";
	
	
	static DatagramPacket createRequest(String method,String uri,int port,String thebody)
	{
		String start_line = method+" "+uri+" "+SIP_VER+"\n";
		String header = "\n";
		String body = thebody+"\n";
		String SIP_msg = new String(start_line+header+"\n"+body);
		
		byte bytearray[] = SIP_msg.getBytes();
		try
		{
			DatagramPacket msg = new DatagramPacket(bytearray,bytearray.length,InetAddress.getByName(uri),port);
			return msg;
		}catch (Exception e){e.printStackTrace();}
		return null;
	}
	
	static DatagramPacket createResponse(String status,String reason,InetAddress uri,int port)
	{
		String start_line = SIP_VER+" "+status+" "+reason+"\n";
		String header = reason+"\n";
		String body = "\n";
		String SIP_msg = new String(start_line+header+"\n"+body);
		
		byte bytearray[] = SIP_msg.getBytes();
		try
		{
			DatagramPacket msg = new DatagramPacket(bytearray,bytearray.length,uri,port);
			return msg;
		}catch (Exception e){}
		return null;
	}
	
	static String[] stripRequestHeader(DatagramPacket msg)
	{
		String SIP_msg = new String(msg.getData());
		
		String a[] = SIP_msg.split("\n");
		
		String header[] = a[0].split(" ");
		
		return header;
	}
	static String stripRequestBody(DatagramPacket msg)
	{
		String SIP_msg = new String(msg.getData());
		
		String a[] = SIP_msg.split("\n");
		
		String body = a[3];
		
		return body;
	}
	static String[] stripResponse(DatagramPacket msg)
	{
		String SIP_msg = new String(msg.getData());
		String a[] = SIP_msg.split("\n");
		/*for (int i=0;i<a.length;i++)
		{
			System.out.println(a[i]);
		}
		if (a.length >= 4)
		{
			System.out.println("invalid message");
			return null;	
		}*/
		String header[] = a[0].split(" ");
		return header;
	}
	
	static String[] stripBody(String body)
	{
		//System.out.println(body);
		String m[] = body.split(",");
		return m;
	}
	static String stripResponseHeader(DatagramPacket msg)
	{
		String SIP_msg = new String(msg.getData());
		String body[] = SIP_msg.split("\n");
		return body[1];
	}
}
