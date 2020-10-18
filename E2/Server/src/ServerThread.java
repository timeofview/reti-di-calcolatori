import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.sql.Timestamp;

public class ServerThread extends Thread {
	private Socket socket;

	public ServerThread(Socket socket) {
		this.socket = socket;
	}

	public void run() {
		int buffer;
		int result;
		String fileName;
		long dim = 0; 
		DataInputStream dataInputStream = null;
		DataOutputStream dataOutputStream = null;
		DataOutputStream outFile = null;
		FileOutputStream out = null;
		
		try {
			System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Thread has been Correctly Initialized with: " + Server.usableSpace + " Usable Space");
			dataOutputStream = new DataOutputStream(socket.getOutputStream());
			dataInputStream = new DataInputStream(socket.getInputStream());
			while (socket.isConnected()) {
				fileName = dataInputStream.readUTF();
				System.out.println("["+new Timestamp(System.currentTimeMillis())+"] File Name: "+ fileName + " has been Correctly Read from Client");
				dim = dataInputStream.readLong();
				System.out.println("["+new Timestamp(System.currentTimeMillis())+"] File Size: "+dim+" has been Correctly Read from Client ");	

				// Deciding if Accepting Request
				if(Files.exists(Paths.get(fileName))) {
					result = -1;
				} else if ((Server.usableSpace-dim)<0) {
					result = -2;
				} else {
					result = 1;
				}

				// Sending result
				dataOutputStream.writeInt(result);
				System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Sending result to Client: " + ((result==1) ? "Accepted" : "Refused"));

				// If Everything is fine
				if (result > 0) {
					
					// Receiving the File Byte per Byte
					out = new FileOutputStream(fileName);
					outFile =  new DataOutputStream(out);
					System.out.println("["+new Timestamp(System.currentTimeMillis())+"] Receiving from Client and Writing");
					// Read Until EOF = -1
					while ((buffer = dataInputStream.read())>0) {
						outFile.write(buffer);
					}
					
					// Closing File
					out.close();
					outFile.flush();
					outFile.close();
					Server.usableSpace-=dim;
					System.out.println("["+new Timestamp(System.currentTimeMillis())+"] A Transfer has been Completed. Usable Space: " + Server.usableSpace);
					//dataOutputStream.writeInt(1);
				}
			}
			//Closing Communications
			socket.shutdownInput();
			socket.shutdownOutput();
			socket.close();
		} catch (Exception e) {
            e.printStackTrace();
		}
	}
}
