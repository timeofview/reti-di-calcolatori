import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.nio.ByteBuffer;

//            0    1       2
// RSClient IPDS portDS fileName
//
// RDC01
public class Client {

	// Costanti
    private final static int TIMEOUT = 30000;
    private final static int MAX_BUF = 256;

    
    /*
  		Sviluppare un’applicazione C/S in cui il client, invocato con l’indirizzo
		IP e la porta su cui il server è in ascolto, invia al server pacchetti
		contenenti il nome del file testo e il numero della linea del file che
		vuole ricevere, che sono richiesti dall’utente usando l’input da console.
		Si noti l’invio di un unico datagramma per più informazioni
		(perché non di due separati?)
	 */
    
    
    public static void main(String[] args) {

        // Controllo argomenti
        if (args.length != 3) {
            System.out.println("Errore: devi passare esattamente 3 argomenti!\nUtilizzo programma: RSClient IPDS portDS fileName");

            // Esco dal programma
            System.exit(0);
        }

        for (int i = 0; i < args.length; i++)
            System.out.println("Parametro " + i + ": " + args[i]);


        // Socket 
        try {
            DatagramPacket packet = null;
            // Creazione Socket
            DatagramSocket socket = new DatagramSocket(); // Creo la socket del client
            socket.setSoTimeout(TIMEOUT); // Setto il timeout massimo
            ByteArrayOutputStream boStream = null;
            DataOutputStream doStream =null;
            ByteArrayInputStream biStream = null;
            DataInputStream diStream = null;
            byte[] buff = new byte[MAX_BUF]; // Creo l'array di byte
            
            // Converto la porta in intero
            int port = -1; 
            try {
               port = Integer.parseInt(args[1]);
            }catch (NumberFormatException e) {
               e.printStackTrace();
            }
            boStream = new ByteArrayOutputStream();
            doStream = new DataOutputStream(boStream);
            doStream.writeUTF(args[2]);
            buff = boStream.toByteArray();
            
            // Ottengo l'ip del Disovery Server
            InetAddress IPAddress = InetAddress.getByName(args[0]);
            // Creo il pacchetto da inviare 
            packet = new DatagramPacket(buff, buff.length, IPAddress, port);
            // Invio il pacchetto
            socket.send(packet);
            

            biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
            diStream = new DataInputStream(biStream);
            packet.setData(buff);
           	// Ottengo la risposta
            socket.receive(packet);
            // Ottengo l'intero che mi descrive la porta
            port = diStream.readInt();
            
            
            // Controllo che la porta ricevuta sia valida 
            if(port < 0) {
        		System.err.println("Errore: il Discovery Server non ha rintracciato un server valido!");
        		System.exit(1);
            }
            
            // Chiedo all'utente i due file
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            
            // Riga 1
            System.out.print("Inserisci la riga 1: ");
            int row1 = -1;
            try{
                row1 = Integer.parseInt(reader.readLine());
            }catch(NumberFormatException e) {
            	System.err.println("Errore: devi inserire un numero di riga!");
				
            	System.exit(0);
            }
            
            // Riga 2
            System.out.print("\nInserisci la riga 2: ");
            int row2 = -1;
            try{
                row2 = Integer.parseInt(reader.readLine());
            }catch(NumberFormatException e) {
            	System.err.println("Errore: devi inserire un numero di riga!");
				
            	System.exit(0);
            }
            
            // Creo i due Stream
            boStream = new ByteArrayOutputStream();
        	doStream = new DataOutputStream(boStream);
        	
        	// Scrivo i due interi nella Stream
            doStream.writeInt(row1);
            doStream.writeInt(row2);
        	
        	// Metto nell'array il contenuto di byteOut
            buff = boStream.toByteArray();
        	
        	// Riempimento del paccheto con i dati
        	packet.setData(buff);
        	
        	// Invio del pacchetto al server
            socket.send(packet);
            
        	//dataOut.close(); // Chiudo il DataOutputStream
        	
            	// Preparo il pacchetto di ricezione e gli passo come contenitore l'array di byte
        	packet.setData(buff);
        	
        	// Ricevo il pacchetto
            socket.receive(packet);
        	
        	// Estrazione delle informazioni dal pacchetto ricevuto
        	biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
        	diStream = new DataInputStream(diStream);
        	
        	// Ottengo la rispsota dal server
        	int result = diStream.readInt();
        	
        	// Stampo a video il risultato
        	System.out.println("Risposta del Server: " + result);
        	
        	// Chiudo la Socket del Client
            socket.close();
        	
            
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


}
