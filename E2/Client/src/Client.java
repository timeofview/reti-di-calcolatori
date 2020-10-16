import java.io.*;
import java.net.*;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Client {
	
	public static void main(String[] args) {

		// Arguments Check
		if (args.length != 2) {
			System.out.println("Usage: Client Address Port");
			return;
		}
		
		int port = -1;
		InetAddress addr;
		
		// Arguments Parsing
		try {
			 addr = InetAddress.getByName(args[0]);
			 port = Integer.parseInt(args[1]);
		} catch (UnknownHostException uhe) {
			uhe.printStackTrace();
			return;
		} catch (NumberFormatException nfe) {
			nfe.printStackTrace();
			return;
		}
		
		Socket socket;
		DataOutputStream dataOutputStream = null;
		DataInputStream dataInputStream = null;
		String dirName = null;
		
		// To Read dirName
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		boolean isDir= false;
		
		// Reading and Checks on dirName
		try {
			while(isDir == false) {
				System.out.println("Enter Directory [EOF to end]: ");
				dirName = stdIn.readLine();
				if(dirName == null) {
					System.out.println("Read EOF ... Closing");
					return;
				}
				if(Files.isDirectory(Paths.get(dirName))) {
						isDir=true;
				} else {
					System.out.println("Path or Name inserted is not a Directory");
				}
			}
			stdIn.close();
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		
		// Communication
		try {
			socket = new Socket(addr, port);
			socket.setSoTimeout(30000);
			dataOutputStream = new DataOutputStream(socket.getOutputStream());
			dataInputStream = new DataInputStream(socket.getInputStream());
			int accepted;
			BufferedReader buff;
			int buffer;
			
			// Contain also Directories
			//File[] files = new File(dirName).list();
			String[] files = new File(dirName).list();
			File f;
			
			// Main Loop
			for(String s : files) {
				if((s.endsWith(".txt"))&&(f = new File(s)).isFile()) {
					String fname = f.getName();
					dataOutputStream.writeUTF(fname);
					dataOutputStream.writeLong(f.length());
					System.out.println("Waing for Server Response...");
					accepted=dataInputStream.readInt();
					if(accepted>=0) {
						buff = new BufferedReader(new FileReader(f));
						
						System.out.println("Reading File and Sending to Server");
						// Read Until EOF = -1
						while((buffer = buff.read())>=0) {
							dataOutputStream.write(buffer);
						}
						
						buff.close();
					} else {
						System.out.println("Server has Refused the Request on File: " + f.getName());
					}
				}
			}
			
			//Closing Communications
			socket.close();
			dataInputStream.close();
			dataOutputStream.close();
			System.out.println("A client has been Served");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
