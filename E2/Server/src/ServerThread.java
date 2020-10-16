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
		int result;
		String fileName;
		long maxDim = 1024;
		long dim = 0; 
		DataInputStream dataInputStream = null;
		DataOutputStream dataOutputStream = null;
		FileOutputStream outFile = null;
		
		try {
			dataInputStream = new DataInputStream(socket.getInputStream());
			dataOutputStream = new DataOutputStream(socket.getOutputStream());
			System.out.println("Thread has been Correctly Initialized");
			
			// Reading from Client
			fileName = dataInputStream.readUTF();
			dim = dataInputStream.readLong();
			System.out.println("fileName has been Correctly Read from Client "+fileName);
			System.out.println("fileDim has been Correctly Read from Client "+dim);
			
			// Deciding if Accepting Request
			if (Files.exists(Paths.get(fileName))||dim>maxDim) {
				result = -1;
			} else {
				result = 1;
			}

			// Sending result
			dataOutputStream.writeInt(result);
			System.out.println("Sending result to Client " + result);
			
			// If Everything is fine
			if (result > 0) {
				
				// Receiving the File Byte per Byte
				outFile = new FileOutputStream(fileName);
				System.out.println("Receiving from Client and Writing");
				// Read Until EOF = -1
				while ((buffer = dataInputStream.read()) >= 0) {
					outFile.write(buffer);
				}
				
				// Closing Communications and Flushing Stream
				outFile.flush();
				outFile.close();
				System.out.println("A task has been Completed");
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
