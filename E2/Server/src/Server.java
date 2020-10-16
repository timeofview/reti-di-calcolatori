
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {

	public static final int PORT = 54321; // porta di default

	public static void main(String[] args) {

		int port = -1;
;
		FileOutputStream outFile = null;
		Byte buffer = null;
		// controllo argomenti
		try {
			if (args.length == 1)
				port = Integer.parseInt(args[0]);
			else if (args.length == 0)
				port = PORT;
			else {
				System.out.println("Usage: Server N_porta(optional)");
				System.exit(1);
			}

		} catch (Exception e) {
			System.out.println("Eccezione durante il controllo degli argomenti");
		}

		long spazioDisponibile = new File("/").getUsableSpace();
		ServerSocket serverSocket = null;
		Socket socket = null;

		try {
			serverSocket = new ServerSocket(port);
			serverSocket.setReuseAddress(true);

		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		while (true) {
			try {
				socket = serverSocket.accept();
				socket.setSoTimeout(30000);
				new ServerThread(socket).start();
			} catch (IOException e) {
				e.printStackTrace();
			}

		}
		
	}

}
