import java.io.*;
import java.net.Socket;



public class ServerThread extends Thread {
	private Socket socket;

	public ServerThread(Socket socket) {
		this.socket = socket;
	}
	
	public void run() {
		System.out.println("FIGLIO: partenza");
		 DataInputStream dis = null;;
		 DataOutputStream dos = null;
		 FileOutputStream fos = null;
		int outcome = -1;
		
		String nameFile; 
		long lenght = 0;
		try {
			try {
				dis = new DataInputStream(socket.getInputStream());
				dos = new DataOutputStream(socket.getOutputStream());
				
			} catch (Exception e) {
				e.printStackTrace();
			}
			System.out.println("FIGLIO: inizio comuncazione con CLient");
			nameFile=dis.readUTF();
			System.out.println("FIGLIO: letto nomeFIle");
			while((nameFile)!=null) {
				
				lenght= dis.readLong();
				
				byte[] b= new byte[1];
				System.out.println("FIGLIO: controllo esistenza del file");
				if(new File(nameFile).exists()){
					outcome=-1;
					System.out.println("FIGLIO: "+nameFile+ " già presente");
					dos.writeInt(outcome);
				}else {
					//inizio trasferimento
					dos.writeInt(1);
					System.out.println("FIGLIO: inizio trasferimento di "+ nameFile);
					fos=new FileOutputStream(new File(nameFile));
					
					while(dis.read(b)>0 ) {
						fos.write(b);
					}
					outcome = 0 ;//esito positivo;
					
					
				}
				dos.writeInt(outcome);
				nameFile=dis.readUTF();
			}
			socket.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
