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
		
		
		Socket s = null;
		DataOutputStream dos = null;
		DataInputStream dis = null;
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
			s = new Socket(addr, port);
			s.setSoTimeout(30000);
			dos=new DataOutputStream(s.getOutputStream());
			dis=new DataInputStream(s.getInputStream());
			int accepted;
		
			File[] files = new File(nameDir).listFiles(); //comprende anche le directory
			InputStream buff;
			byte[] b= new byte[1];
			for(File f : files) {
				
				if(f.isFile() && !f.isHidden()) {
					System.out.println("CLIENT: invio nome file "+ f.getName());
					dos.writeUTF(f.getName());
					dos.writeLong(f.length());
					System.out.println("Attendo risposta dal Server...");
					accepted=dis.readInt();
					
					if(accepted>=0) {
						System.out.println("richiesta accettata: invio del file "+ f.getName());
						buff = new FileInputStream(f);
						while(((buff.read(b))>0)) { // da ragionare
							dos.write(b);
						}
						dis.readInt();
						buff.close();
					}else {
						System.out.println("Il server ha rifiutato l'operazione sul file "+f.getName());
					}
				}
			}
			
			
			s.close();
			dis.close();
			dos.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
		
	}
}
