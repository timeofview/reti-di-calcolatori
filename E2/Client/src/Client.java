import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.Timestamp;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class Client {

    private static final int MAX_TIME = 30000;

    public static void main(String[] args) {


		// Arguments Check
        if (args.length != 3) {
            System.err.println("["+new Timestamp(System.currentTimeMillis())+"] Usage: Client Address Port");
            System.exit(1);
        }

        int port = -1;
		InetAddress addr = null;
		int minDim = 0;
		
		// Arguments Parsing
		try {
			 addr = InetAddress.getByName(args[0]);
			 port = Integer.parseInt(args[1]);
			 minDim = Integer.parseInt(args[2]);
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
                System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Enter Directory [EOF to end]: ");
                dirName = reader.readLine();
            } while (!Files.isDirectory(Paths.get(dirName)));
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(4);
        }

        System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Directory: " + dirName + " Accepted");

        // Communication
        Socket clientSocket;
        DataOutputStream dataOutputStream=null;
        DataInputStream dataInputStream=null;
        DataInputStream inFile;

        // Creating Files to represent Directory
        File dir = new File(dirName);

        // List Files except Sub-Directory
        File[] filesInDir = dir.listFiles(new FileFilter() {
            @Override
            public boolean accept(File file) {
                //Filtering files that  are not hidden
                return file.isFile() && !file.isHidden();
            }
        });
        try {
			reader.close();
		} catch (IOException e1) {
			e1.printStackTrace();
		}

        int response = -1;
        int count = 0;
        long record = 0;
        try {

            // Socket Creation and Timeout Setting
            clientSocket = new Socket(addr, port);
            //clientSocket.setSoTimeout(MAX_TIME);

            // Streams
            dataOutputStream = new DataOutputStream(clientSocket.getOutputStream());
            dataInputStream = new DataInputStream(clientSocket.getInputStream());

            // For Each File
            for (File f: filesInDir) {
            	//Check Dimension
            	if(f.length()<minDim||(f.length()>=record&&record!=0)) {continue;}
                // Writing details to Server
                dataOutputStream.writeUTF(f.getName());
                dataOutputStream.writeLong(f.length());
                System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Informations about the File: " + f.getName() + " of: " + f.length() + " Lenght has been Sent to Server");

                // Waiting for a response from Server
                System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Waiting for Server Response...");
                response = dataInputStream.readInt();

                // If Accepted
                if (response == -2) {
                	record = f.length();
                }
                else if (response >= 0) {

                    System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Server has accepted the request of File: " + f.getName());

                    // BufferedReader Initialisation
                    System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Reading File: " + f.getName());
                    inFile = new DataInputStream(new FileInputStream(f));
                    System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Sending File: " + f.getName() + " to Server");

                    // Read and Write till EOF (-1)
                    while ((count = inFile.read())>0) {
                        dataOutputStream.write(count);
                    }
                    dataOutputStream.write(0);
                    System.out.println("["+new Timestamp(System.currentTimeMillis())+"] File: " + f.getName() + " Sent");
                    /*if((response = dataInputStream.readInt())==1) {System.out.println("Server has completed Operation on File");}
                    else {System.out.println("Server hasn't completed Operation on File");}*/
                    dataOutputStream.flush();
                } else {
                    System.err.println("["+new Timestamp(System.currentTimeMillis())+"] Server has refused the request of File: " + f.getName());
                }
            }

            // Closing Communications
            clientSocket.shutdownOutput();
            clientSocket.shutdownInput();
            clientSocket.close();
            dataOutputStream.close();
            dataInputStream.close();
            System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Closing Communications");
            System.out.println("["+new Timestamp(System.currentTimeMillis())+"] A Client has been Served");
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(5);
        }
    }
}
