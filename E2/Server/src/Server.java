
import java.io.*;
import java.net.*;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Server {

	public static final int PORT = 54321; // porta di default

	public volatile long spazioDisponibile = new File("/").getUsableSpace(); // e se non la mettessimo???
	
	public static void main(String[] args) {

		int port = -1;
		String nomeFile;
		long dim;
		int esito = 0;
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

		ServerSocket serverSocket = null;

		
		Socket s = null;
		DataInputStream dis = null;
		DataOutputStream dos = null;
		try {
			serverSocket = new ServerSocket(port);

			serverSocket.setReuseAddress(true);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}

		// creazione della socket per la comunicazione
		while (true) {
			try {
				s = serverSocket.accept();
				s.setSoTimeout(30000);
			} catch (IOException e) {
				e.printStackTrace();
			}

			// creazione metodi/stream per l I\O con la socket
			try {
				dis = new DataInputStream(s.getInputStream());
				dos = new DataOutputStream(s.getOutputStream());

				// leggo cosa mi ha inviato il Client
				nomeFile = dis.readUTF();
				dim = dis.readLong();
				// controllo per vedere se accettare l'invio del file da parte del Client (FIGLIO)
				if (Files.exists(Paths.get(nomeFile))) { //FIX ME PLEASE
					esito = -1;
				} else if (dim >= spazioDisponibile) {
					esito = -2;
				} else {
					esito = 1;
				}

				dos.writeInt(esito);
				if (esito > 0) {
					outFile = new FileOutputStream(nomeFile);
					// ricezione File byte per byte

					while ((buffer = dis.readByte()) >= 0) { // EOF valore negativo
						outFile.write(buffer);
					}
					outFile.flush();

					spazioDisponibile -= dim;
					outFile.close();
					//dos.writeUTF("file " + nomeFile + " salvato con successo...ESITO: " + esito);

				}
				outFile.close();
				dis.close();
				dos.close();
				
				s.close();
				serverSocket.close();
			} catch (IOException e) {
				e.printStackTrace();
			}

		}

	}
}
