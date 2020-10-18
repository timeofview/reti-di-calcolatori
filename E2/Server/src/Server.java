import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

public class Server {

    // Default timeout and port
    public static final int DEFAULT_PORT = 54321;
    private static final int MAX_TIME = 30000;
    
    public static volatile long usableSpace = 1048576; 
    
    public static void main(String[] args) {
    	
        int port = -1;
        
        // Arguments check
        try {
            if (args.length == 1)
                port = Integer.parseInt(args[0]);
            else if (args.length == 0)
                port = DEFAULT_PORT;
            else {
                System.out.println("Invalid argument number!\nProgram usage: Server PortNumber[Optional]");
                System.exit(1);
            }
        } catch (NumberFormatException nfe) {
            nfe.printStackTrace();
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

        Socket clientSocket;
        System.out.println("Server has started");
        
        // Main loop
        while (true) {
            try {
            	
            	// Accepting client connections 
                clientSocket = serverSocket.accept();
                clientSocket.setSoTimeout(MAX_TIME);
                
                // Starting a ServerThread
                new ServerThread(clientSocket).start();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
