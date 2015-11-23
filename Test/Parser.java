import java.util.*;
import java.io.*;
import java.io.IOException;
import java.lang.String.*;

public class Parser {



	public Parser()
	{


	}//END CONSTRUCTOR


	public static int readFromFile(String file)
	{
		String fileName = file;
		String line = null;
		String delims = "[ ]+";
		int previousCount=0;
		int lineNumber=0;
		
		try 
		{
			FileReader fileReader = new FileReader(fileName);
			BufferedReader bufferedReader = new BufferedReader(fileReader);
			while((line = bufferedReader.readLine()) != null) 
			{
				line.trim();
				lineNumber++;
				if(line.startsWith("The Node Entering CS is "))
				{
					int newCount=0;
					String[] enterTokens = line.split(delims);		
					for(int i=9;i<enterTokens.length;i++)
					{
						newCount = newCount+Integer.parseInt(enterTokens[i]);
					}
					if(newCount!=(previousCount+1))
					{
						System.out.println("The Processes are not executing Critical Sections in order. See line "+ lineNumber);
						return 1;
					}
					else 
					{
						previousCount=newCount;
					}
					line = bufferedReader.readLine();
					lineNumber++;
					if(line.startsWith("The Node Exiting CS is "))
					{
						String[] exitTokens = line.split(delims);
						if(Integer.parseInt(enterTokens[5])!=Integer.parseInt(exitTokens[5]))
						{
							System.out.println(Integer.parseInt(enterTokens[5]) + " is overlapping with " + Integer.parseInt(exitTokens[5]) + " at line " + lineNumber);
							return 1;
						}


					}
					else if(line.startsWith("The Node Entering CS is "))
					{
						System.out.println("There are overlapping Critical Sections at line " + lineNumber);
						return 1;
					}
					else 
					{
						System.out.println("Please check your Parser. There is something wrong with it at line " + lineNumber);
						return 1;
					}//END EXIT
				

				}//END ENTER

			}//END WHILE
		}//END TRY 
		catch(Exception e)
		{
			e.printStackTrace();
		}//END CATCH 	
		
		System.out.println("Everything went perfect! Code is correct !");
		return 1;

	}//END READING FROM FILE 






	public static void main(String [] args)
	{
		
		if(args.length !=1)
		{
			System.out.println("You goofed !");
			System.exit(0);
		}
		int call= readFromFile(args[0]);





	}//END MAIN 








}// END 
