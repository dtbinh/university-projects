import java.net.*;
import java.io.*;
import java.security.*;
import java.util.*;

public class REP_MSG implements Serializable
{
    // variables needed in creating the reply message
	double id = 0;
	String resp = null;
	long time = 0;
	boolean valid = false;
	boolean timeCheck = false;
	PublicKey publicKey = null;
	REP_MSG next = null;
	
        /* class constructor used to create the message. To create a message in any 
         * class which imports this message class, one only needs to call the constructor
         * with the correct parameters. At this point in the project, only the Object resp
         * seems to be of importance.
         */
	public REP_MSG(double id, PublicKey publicKey,boolean valid,boolean timeCheck)
	{
		this.id = id;
		this.publicKey = publicKey;
		this.valid = valid;
		this.timeCheck = timeCheck;
	}
	
	public REP_MSG(String string)
	{
		resp = string;
	}
	
        // a procedure to display the message
	public void print_msg()
	{	
		System.out.println("id: "+id);
		System.out.println(resp);
		System.out.println(time);
	}
	
	public void setNext(REP_MSG next)
	{
		this.next = next;
	}
	

}