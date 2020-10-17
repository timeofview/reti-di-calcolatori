
import java.io.IOException;
import java.net.*;

public class ServerParent {


		public static final int PORT = 54321; // porta di default

		//public volatile long spazioDisponibile = new File("/").getUsableSpace(); // e se non la mettessimo???
		
		public static void main(String[] args) {
			
			int port=-1;
			
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
			
			
			//inizializzo variabili
			
			Socket socket= null;
			ServerSocket serverSocket = null;
			try {
				serverSocket = new ServerSocket(port); 
					
				serverSocket.setReuseAddress(true);
			} catch (IOException e1) {
				e1.printStackTrace();
			}
			
			
			while(true) {
				
				try {
					socket = serverSocket.accept();
					socket.setSoTimeout(30000);
					System.out.println("creazione figlio");
					new ServerThread(socket).start();
					//tocca al figlio chiudere la socket
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					//System.exit(2);
					
				} 
			}
			
			
		}
}
