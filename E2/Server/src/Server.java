import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

public class Server {

	// Default Port
	public static final int PORT = 54321; 
    public static volatile long usableSpace = 1048576; 

	public static void main(String[] args) {
		int port = -1;
		// Arguments Check
		try {
			if (args.length == 1)
				port = Integer.parseInt(args[0]);
			else if (args.length == 0)
				port = PORT;
			else {
				System.out.println("Usage: Server portNumber[optional]");
				System.exit(1);
			}
		} catch (Exception e) {
			System.out.println("Exception during Arguments Check");
		}

		ServerSocket serverSocket = null;

		// Server Socket Creation
		try {
			serverSocket = new ServerSocket(port);
			serverSocket.setReuseAddress(true);
		} catch (SocketException se) {
			se.printStackTrace();
			System.exit(1);
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(2);
		}
		
		Socket socket;
		
		System.out.println("Server has been Correctly Initialized");
		
		// Main Loop
		while (true) {
			try {
				socket = serverSocket.accept();
				socket.setSoTimeout(30000);
				System.out.println("Socket has been Correctly Accepted");
				new ServerThread(socket).start();
			} catch (IOException e) {
				e.printStackTrace();
				//Doesn't Exit
			}
		}	
	}
}
