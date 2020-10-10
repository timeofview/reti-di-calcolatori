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
public class RSClient {

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
        	
            // Creazione Socket
            DatagramSocket client = new DatagramSocket(); // Creo la socket del client
            client.setSoTimeout(TIMEOUT); // Setto il timeout massimo
            
            byte[] sendData = new byte[MAX_BUF]; // Creo l'array di byte
            sendData = args[2].getBytes(); // Ottengo i byte contenenti il nomeFile
            
            
            // Converto la porta in intero
            int port = -1; 
            try {
               port = Integer.parseInt(args[1].trim());
            }catch (NumberFormatException e) {
               e.printStackTrace();
            }
            
            
            // Ottengo l'ip del Disovery Server
            InetAddress IPAddress = InetAddress.getByName(args[0]);
            // Creo il pacchetto da inviare 
            DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, port);
            // Invio il pacchetto
            client.send(sendPacket);
            
            
            // Creo l'array di byte contente il messaggio del Discovery Server
            byte[] receiveData = new byte[MAX_BUF];
            // Creo il pacchetto contenente la risposta
            DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
           	// Ottengo la risposta
            client.receive(receivePacket);
            // Ottengo l'intero che mi descrive la porta
            int receivePort = ByteBuffer.wrap(receiveData).getInt();
            
            
            // Controllo che la porta ricevuta sia valida 
            if(receivePort < 0) {
        		System.err.println("Errore: il Discovery Server non ha rintracciato un server valido!");
				
        		System.exit(0);
            }
            
            // Chiedo all'utente i due file
            BufferedReader brUserInput = new BufferedReader(new InputStreamReader(System.in));
            
            // Riga 1
            System.out.print("Inserisci la riga 1: ");
            int numRigaUno = -1;
            try{
            	numRigaUno = Integer.parseInt(brUserInput.readLine().trim());
            }catch(NumberFormatException e) {
            	System.err.println("Errore: devi inserire un numero di riga!");
				
            	System.exit(0);
            }
            
            // Riga 2
            System.out.print("\nInserisci la riga 2: ");
            int numRigaDue = -1;
            try{
            	numRigaUno = Integer.parseInt(brUserInput.readLine().trim());
            }catch(NumberFormatException e) {
            	System.err.println("Errore: devi inserire un numero di riga!");
				
            	System.exit(0);
            }
            
            // Creo i due Stream
            ByteArrayOutputStream byteOut = new ByteArrayOutputStream();
        	DataOutputStream dataOut = new DataOutputStream(byteOut);
        	
        	// Scrivo i due interi nella Stream
        	dataOut.writeInt(numRigaUno);
        	dataOut.writeInt(numRigaDue);
        	
        	// Metto nell'array il contenuto di byteOut
        	sendData = byteOut.toByteArray(); 	
        	
        	// Riempimento del paccheto con i dati
        	sendPacket.setData(sendData);
        	
        	// Invio del pacchetto al server
        	client.send(sendPacket);
            
        	//dataOut.close(); // Chiudo il DataOutputStream
        	
            	// Preparo il pacchetto di ricezione e gli passo come contenitore l'array di byte
        	receivePacket.setData(receiveData);
        	
        	// Ricevo il pacchetto
        	client.receive(receivePacket);
        	
        	// Estrazione delle informazioni dal pacchetto ricevuto
        	ByteArrayInputStream byteIn = new ByteArrayInputStream(receivePacket.getData(), 0, receivePacket.getLength());
        	DataInputStream dataIn = new DataInputStream(byteIn);
        	
        	// Ottengo la rispsota dal server
        	int esitoOperazione = dataIn.readInt();
        	
        	// Stampo a video il risultato
        	System.out.println("Risposta del Server: " + esitoOperazione);
        	
        	// Chiudo la Socket del Client
        	client.close(); 
        	
            
        } catch (IOException e) {
            e.printStackTrace();
        }

    }


}
