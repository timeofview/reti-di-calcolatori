import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
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
		long maxDim = Long.MAX_VALUE;
		long dim = 0;
		DataInputStream dataInputStream = null;
		DataOutputStream dataOutputStream = null;
		FileOutputStream outFile = null;

		try {
			dataInputStream = new DataInputStream(socket.getInputStream());
			dataOutputStream = new DataOutputStream(socket.getOutputStream());
			System.out.println("Thread has been Correctly Initialized with: " + Server.usableSpace + " Usable Space");
			while (!socket.isClosed()) {

				fileName = dataInputStream.readUTF();
				System.out.println("File Name: " + fileName + " has been Correctly Read from Client");
				dim = dataInputStream.readLong();
				System.out.println("fileDim has been Correctly Read from Client " + dim);

				// Deciding if Accepting Request
				if (Files.exists(Paths.get(fileName)) || dim > maxDim || (Server.usableSpace - dim) < 0) {
					result = -1;
				} else {
					result = 1;
				}

				// Sending result
				dataOutputStream.writeInt(result);
				System.out.println("Sending result to Client: " + ((result == 1) ? "Accepted" : "Refused"));

				// If Everything is fine
				if (result > 0) {

					// Receiving the File Byte per Byte
					outFile = new FileOutputStream(fileName);
					System.out.println("Receiving from Client and Writing");
					// Read Until EOF = -1
					while ((buffer = dataInputStream.read()) > 0) {
						outFile.write(buffer);
					}
					outFile.flush();
					outFile.close();
					
					Server.usableSpace -= dim;
					System.out.println("A Transfer has been Completed. Usable Space: " + Server.usableSpace);
				}
			}

		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				// Closing Communications and Flushing Stream

				socket.shutdownInput();
				socket.shutdownOutput();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
