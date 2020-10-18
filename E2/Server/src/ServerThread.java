import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;

public class ServerThread extends Thread {
    private Socket socket;

    public ServerThread(Socket socket) {
        this.socket = socket;
    }

    public void run() {
    	
        int buffer;
        int response;
        String fileName;
        long fileLength = 0;
        DataInputStream dataIn = null;
        DataOutputStream dataOut = null;
        DataOutputStream fileOut = null;
        FileOutputStream out = null;

        try {
            System.out.println("A server thread has been correctly initialized with " + Server.usableSpace + " available storage");
            
            dataOut = new DataOutputStream(socket.getOutputStream());
            dataIn = new DataInputStream(socket.getInputStream());
            
            // Main loop
            while (socket.isConnected()) {
            	
            	// Reading name and length of File
                fileName = dataIn.readUTF();
                fileLength = dataIn.readLong();

                // "response" will contain the server response
                if (Files.exists(Paths.get(fileName))) {
                	
                	// Server already's got the file :(
                    response = -1;
                } else if ((Server.usableSpace - fileLength) < 0) {
                	
                	// Server storage is full :(
                    response = -2;
                } else {
                	
                	// OK, accepting the request :)
                    response = 1;
                }

                // Sending "response"
                dataOut.writeInt(response);

                // If the client request has been accepted
                if (response > 0) {

                    out = new FileOutputStream(fileName);
                    fileOut = new DataOutputStream(out);

                    // Reading and writing byte per byte until EOF = -1 is reached
                    while ((buffer = dataIn.read()) > 0) {
                        fileOut.write(buffer);
                    }

                    // Closing file
                    out.close();
                    fileOut.flush();
                    fileOut.close();
                    
                    Server.usableSpace -= fileLength;
                    System.out.println("A file transfer has been completed. Server storage: " + Server.usableSpace);
                }
            }
            
            // Closing communication
            socket.shutdownInput();
            socket.shutdownOutput();
            socket.close();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
