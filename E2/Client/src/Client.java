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


public class Client {

    private static final int MAX_TIME = 30000;

    public static void main(String[] args) {


        // Arguments check
        if (args.length != 3) {
            System.err.println("Incorrect number of arguments!\nProgram usage: Client Address Port MinFileSize");
            System.exit(1);
        }

        int port = -1;
        int minDim = 0;

        // Arguments parsing
        try {
            port = Integer.parseInt(args[1]);
            minDim = Integer.parseInt(args[2]);
        } catch (NumberFormatException nfe) {
            nfe.printStackTrace();
            System.exit(2);
        }

        // Read dirName
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String dirName = null;

        // User input loop
        try {
            do {
                System.out.print("Enter a valid non-empty Directory [EOF to end]: ");
                dirName = reader.readLine();
            } while (!Files.isDirectory(Paths.get(dirName)) || !Files.list(Paths.get(dirName)).findAny().isPresent());
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(3);
        }


        // Closing BufferedReader
        try {
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(4);
        }

        // Communication
        Socket clientSocket;
        DataOutputStream dataOut = null;
        DataInputStream dataIn = null;
        DataInputStream fileIn;

        // Creating a "File" object that represents the Directory selected by the user
        File dir = new File(dirName);

        // List all files in the Directory, except sub-directories and hidden files
        File[] filesInDir = dir.listFiles(new FileFilter() {
            @Override
            public boolean accept(File file) {

                // FileFilter needed to cut off sub-directories and hidden files
                return file.isFile() && !file.isHidden();
            }
        });

        // Comunication start
        int response = -1;
        int count = 0;
        long record = 0;
        try {

            // Socket creation and timeout setting
            clientSocket = new Socket(args[0], port);
            clientSocket.setSoTimeout(MAX_TIME);

            // Streams
            dataOut = new DataOutputStream(clientSocket.getOutputStream());
            dataIn = new DataInputStream(clientSocket.getInputStream());

            // Iterate on each file in the directory 
            for (File f: filesInDir) {

                // Check dim 
                if (f.length() < minDim || (f.length() >= record && record != 0)) {
                    continue;
                }
                // Sending the name and length of the file
                dataOut.writeUTF(f.getName());
                dataOut.writeLong(f.length());

                // Waiting server response
                response = dataIn.readInt();

                // If the server accepts the client request
                if (response == -2) {
                    record = f.length();
                } else if (response >= 0) {

                    System.out.println("Server has accepted file " + f.getName() + "!");

                    // FileInputStream initialization
                    fileIn = new DataInputStream(new FileInputStream(f));

                    // Reading and writing byte per byte untill EOF (-1) is reached
                    while ((count = fileIn.read()) > 0) {
                        dataOut.write(count);
                    }

                    // Writing the end of each byte stream (needed, otherwise it won't work)
                    dataOut.write(0);

                    // Sending the content of dataOut
                    dataOut.flush();

                } else {
                    System.err.println("Server has refused file " + f.getName() + "!");
                }
            }

            // Closing communication
            clientSocket.shutdownOutput();
            clientSocket.shutdownInput();
            clientSocket.close();
            dataOut.close();
            dataIn.close();

        } catch (IOException e) {
            e.printStackTrace();
            System.exit(5);
        }
    }
}
