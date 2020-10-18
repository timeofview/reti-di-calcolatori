import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Client {

    private static final int MAX_TIME = 30000;

    public static void main(String[] args) {


		// Arguments Check
        if (args.length != 2) {
            System.err.println("Usage: Client Address Port");
            System.exit(1);
        }

        int port = -1;
		InetAddress addr = null;
		
		// Arguments Parsing
		try {
			 addr = InetAddress.getByName(args[0]);
			 port = Integer.parseInt(args[1]);
		} catch (UnknownHostException uhe) {
			uhe.printStackTrace();
            System.exit(2);
		} catch (NumberFormatException nfe) {
			nfe.printStackTrace();
            System.exit(3);
		}

        // To Read dirName
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String dirName = null;

        // Input Read Loop
        try {
            do {
                System.out.println("Enter Directory [EOF to end]: ");
                dirName = reader.readLine();
            } while (!Files.isDirectory(Paths.get(dirName)));
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(4);
        }

        System.out.println("Directory: " + dirName + " Accepted");

        // Communication
        Socket clientSocket;
        DataOutputStream dataOut;
        DataInputStream dataIn;
        DataInputStream fileStream;
        
        // Creating Files to represent Directory
        File dir = new File(dirName);

        // List Files except Sub-Directory
        File[] filesInDir = dir.listFiles(new FileFilter() {
            @Override
            public boolean accept(File file) {
                //Filtering files that  are not hidden
                return file.isFile();
            }
        });

        int response, count = -1;
        try {

            // Socket Creation and Timeout Setting
            clientSocket = new Socket(addr, port);
            //clientSocket.setSoTimeout(MAX_TIME);

            // Streams
            dataOut = new DataOutputStream(clientSocket.getOutputStream());
            dataIn = new DataInputStream(clientSocket.getInputStream());

            // For Each File
            for (File f: filesInDir) {


                // Writing details to Server
                dataOut.writeUTF(f.getName());
                dataOut.writeLong(f.length());
                System.out.println("Informations about the File: " + f.getName() + " of: " + f.length() + " Lenght has been Sent to Server");
                
                // Waiting for a response from Server
                System.out.println("Waiting for Server Response...");
                response = dataIn.readInt();

                // If Accepted
                if (response >= 0) {
                    System.out.println("Server has accepted the request of File: " + f.getName());

                    // BufferedReader Initialization
                    System.out.println("Reading File: " + f.getName());
                    fileStream = new DataInputStream(new FileInputStream(f));
                    System.out.println("Sending File: " + f.getName() + " to Server");

                    // Read and Write till EOF (-1)
                    while ((count = fileStream.read()) > 0) {
                        dataOut.write(count);
                    }
                    dataOut.write(0);
                    dataOut.flush();

                    System.out.println("File: " + f.getName() + " Sent");
                    reader.close();
                } else {
                    System.err.println("Server has refused the request of File: " + f.getName());
                }
              //  response = dataIn.readInt();
                System.out.println(response);
            }

            // Closing Communications
            clientSocket.close();
            dataOut.close();
            dataIn.close();
            System.out.println("Closing Communications");
            System.out.println("A Client has been Served");
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(5);
        }
    }
}
