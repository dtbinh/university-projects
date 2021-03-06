package project3;

/**
 * A node is identified by its name, a single uppercase character.
 * Conversions to/from characters are handled by 
 * {@link #getName()} and {@link #valueOf(char)}, respectively.
 * <p>
 * Package members are also given access to an identity relationship between
 * nodes and numbers: they can converts between <code>node</code>
 * instances and numbers using {@link #valueOf(int)} and {@link #getIndex()}.
 */

public final class node implements Comparable
{
	/**
	 * The largest possible number of nodes.
	 */
	public static final int MAX_NUMBER = 26;
	
	private static final node[] nodes = new node[MAX_NUMBER];

	static
	{
        // initialize all node objects
		for (char c = 'A'; c <= 'Z'; c++)
		{
			nodes[getIndexForName(c)] = new node(c);
		}		
	}
		
	private static int getIndexForName(char name)
	{
		return name - 'A';
	}	
	
	private static char getNameForIndex(int index)
	{
		return (char)('A' + index);
	}	
	
	public static final node A = node.valueOf('A');
	public static final node B = node.valueOf('B');
	public static final node C = node.valueOf('C');
	public static final node D = node.valueOf('D');
	public static final node E = node.valueOf('E');
    public static final node F = node.valueOf('F');
	
	public static node valueOf(char name)
	{
		if (name < 'A' || name > 'Z')
		{
			throw new IllegalArgumentException("Invalid city name: " + name);	
		}
		
		return nodes[getIndexForName(name)];
	}
	
    /*
     * Package members only.
     */
	static node valueOf(int n)
	{
		if (n < 0 || n > 25)
		{
			throw new IllegalArgumentException("Invalid city number: " + n);
		}
		
		return valueOf( getNameForIndex(n) );		
	}

	private final char name;
	
    /**
     * Private constructor.
     * @param name
     */
	private node(char name)
	{
		this.name = name;	
	}

	public char getName()
	{
		return name;	
	}
	
    /*
     * Package members only.
     */
	int getIndex()
	{
		return getIndexForName(name);
	}	
	
    /**
     * @see java.lang.Object#toString()
     */
	public String toString()
	{
		return String.valueOf(name);
	}

    /**
     * Two nodes are considered equal if they are the same object,
     * or their names are the same.
     * 
     * @see java.lang.Object#equals(Object)
     */    
    public boolean equals(Object o)
    {
        return this == o || equals((node) o);
    }
    
    private boolean equals(node c)
    {
        return this.name == c.name;
    }

    /**
     * Compare two nodes by name.
     * 
     * @see java.lang.Comparable#compareTo(Object)
     */
    public int compareTo(Object o)
    {
        return compareTo((node) o);
    }
    
    public int compareTo(node c)
    {
        return this.name - c.name;
    }
}
