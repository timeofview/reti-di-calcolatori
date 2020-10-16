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
		int buffer = 0;
		int result;
		String fileName;
		long dim = 9999999999L;
		DataInputStream dataInputStream = null;
		DataOutputStream dataOutputStream = null;
		FileOutputStream outFile = null;
		try {
			dataInputStream = new DataInputStream(socket.getInputStream());
			dataOutputStream = new DataOutputStream(socket.getOutputStream());

			// leggo cosa mi ha inviato il Client
			fileName = dataInputStream.readUTF();
			dim = dataInputStream.readLong();
			
			// controllo per vedere se accettare l'invio del file da parte del Client
			if (Files.exists(Paths.get(fileName))) {
				result = -1;

			} else {
				result = 1;
			}

			dataOutputStream.writeInt(result);
			if (result > 0) {
				outFile = new FileOutputStream(fileName);
				// ricezione File byte per byte
				int count = 0;
				while ((buffer = dataInputStream.read()) >= 0) { // EOF valore negativo
					outFile.write(buffer);
				}
				outFile.flush();

				outFile.close();

			}

		} catch (Exception e) {
			e.printStackTrace();
		}

	}
}
