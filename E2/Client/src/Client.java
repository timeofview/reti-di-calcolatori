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
			dos=new DataOutputStream(new BufferedOutputStream(s.getOutputStream()));
			dis=new DataInputStream(new BufferedInputStream(s.getInputStream()));
			int accettato;
			long length=0;
			File[] files = new File(nameDir).listFiles(); //comprende anche le directory
			BufferedReader buff;
			String temp;
			for(File f : files) {
				if(f.isFile()) {
					length = f.length();
					dos.writeUTF(nameDir);
					dos.writeLong(length);
					System.out.println("Attendo risposta dal Server...");
					accettato=dis.read();
					if(accettato>=0) {
						buff = new BufferedReader(new FileReader(f));
						while((temp = buff.readLine())!=null) {
							dos.writeUTF(temp);
						}
						
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
