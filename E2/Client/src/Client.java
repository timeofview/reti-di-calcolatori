import java.io.*;
import java.net.*;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Client {
	
	public static void main(String[] args) {
		//CONTROLLO ARGOMENTI
		
		if (args.length != 2) {
			System.out.println("Usage: Client Address Port");
			return;
		}
		
		
		
		int port=-1;
		InetAddress addr = null;
		
		try {
			 addr = InetAddress.getByName(args[0]);
			 port = Integer.parseInt(args[1]);
		} catch (UnknownHostException e) {
			e.printStackTrace();
			return;
		}
		
		
		Socket socket = null;
		DataOutputStream dataOutputStream = null;
		DataInputStream dataInputStream = null;
		String nameDir = null;
		
		
		//per ricevere il nome della directory su cui lavorare;
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		boolean isDir= false;
		try {
			while(isDir==false) {
				System.out.println("Dichiarare la Directory (EOF per terminare) : ");
				nameDir = stdIn.readLine();
				if(nameDir == null) {
					System.out.println("Selezionato EOF...terminazione");
					return;
				}
				if(Files.isDirectory(Paths.get(nameDir)))
						isDir=true;
				else
					System.out.println("nome o Path passato non corrisponde ad una directory");
				
				
			}
			stdIn.close();
		} catch (IOException e) {
			
			e.printStackTrace();
			return;
		}
		
		//COMUNICAZIONE
		
		try {
			socket = new Socket(addr, port);
			socket.setSoTimeout(30000);
			dataOutputStream=new DataOutputStream(socket.getOutputStream());
			dataInputStream=new DataInputStream(socket.getInputStream());
			int accepted;
		
			File[] files = new File(nameDir).listFiles(); //comprende anche le directory
			BufferedReader buff;
			String temp;
			for(File f : files) {
				if(f.isFile()) {
					String fname = f.getName();
					dataOutputStream.writeUTF(fname);
					dataOutputStream.writeLong(f.length());
					System.out.println("Attendo risposta dal Server...");
					if(accepted>=0) {
					accepted=dataInputStream.readInt();
						buff = new BufferedReader(new FileReader(f));
							dataOutputStream.writeUTF(temp);
						while((temp = buff.readLine())!=null) { // da ragionare
						}
						
						buff.close();
					}else {
						System.out.println("Il server ha rifiutato l'operazione sul file "+f.getName());
					}
				}
			}
			
			
			socket.close();
			dataInputStream.close();
			dataOutputStream.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
		
	}
}
