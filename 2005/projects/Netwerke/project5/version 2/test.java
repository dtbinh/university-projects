public class test
{
	public static void main(String[] args)
	{
		String a = SIP.createRequest("INVITE","localhost");
		System.out.println(a+"Now stripping");
		String header[] = SIP.stripRequest(a);
		for (int i=0;i<header.length;i++)
		{
			System.out.println(header[i]);
		}
		String b = SIP.createResponse("100","jou_ma");
		System.out.println(b+"Now stripping");
		String header2[] = SIP.stripResponse(b);
		for (int i=0;i<header2.length;i++)
		{
			System.out.println(header2[i]);
		}
	}
}