public class list
{
	private list next,prev;
	private String actual, virtual;
	byte[] arp = new byte[28];
	private long time;
	
	public list(String actual, String virtual, byte[] buf)
	{
		this.actual = actual;
		this.virtual = virtual;
		for (int i=0;i<28;i++)
		{
			this.arp[i] = buf[i];
		}
		this.next = null;
		this.prev = null;
		time = System.currentTimeMillis();
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
	public String returnIp()
	{
		return virtual;
	}
	public byte[] getArp()
	{
		return arp;
	}
	public long getTime()
	{
		return time;
	}
	public void refreshTime()
	{
		time = System.currentTimeMillis();
	}
	public String getActual()
	{
		return actual;
	}
}