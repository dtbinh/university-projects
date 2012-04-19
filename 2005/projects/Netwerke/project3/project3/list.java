package project3;

import java.net.*;

/**
 * Tik hier stuff oor list
 *
 */
 
public class list
{
	private list next,prev;
	private Socket socka;
	private String len;
	private int a,b,w,local,remote;
	
	
	public list(Socket socka, String len,int local, int remote)
	{
		this.socka = socka;
		this.len = len;
		this.remote = remote;
		this.local = local;
		
		this.next = null;
		this.prev = null;
		
	}
	public list(int a, int b, int weight)
	{
		this.a = a;
		this.b = b;
		this.w = weight;
		
		this.next = null;
		this.prev = null;
		
	}
	public int a()
	{
		return a;
	}
	public int b()
	{
		return b;
	}
	public int w()
	{
		return w;
	}
	public void setNext(list next)
	{
		this.next = next;
	}
	public void setPrev(list prev)
	{
		this.prev = prev;
	}
	public list getNext()
	{
		return next;
	}
	public list getPrev()
	{
		return prev;
	}
	public String returnSa()
	{
		return "Router " + (remote - 5000) + " to me(Router " + (local-5000)+"), weight = "+len;
	}
	public Socket returnSocka()
	{
		return socka;
	}
	public String returnLen()
	{
		return len;
	}
	public String host()
	{
		return (socka.getInetAddress()).getHostName();
	}
	public int port()
	{
		return socka.getPort();
	}
}