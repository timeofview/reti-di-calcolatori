import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Client {

	private static final int MAX_TIME = 30000;
	
	
	public static void main(String[] args) {
		
		
		// Controllo argomenti
		if(args.length != 2) {
			System.err.println("Numero degli argomenti non corretto!\nUso programma: Client Indirizzo Porta");
			System.exit(1);
		}
		
		
		// Conversione argomenti
		int port = -1;
		try {
			port = Integer.parseInt(args[1]);
		}catch(NumberFormatException e) {
			System.err.println("Errore: impossibile convertire la porta in intero!");
			System.exit(2);
		}
        
		
        // Lettura Input utente
		// Continuo a chiedere la Directory finché non ne viene data una corretta
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));;
        String inputDirName = null;
    	try {
	        do {
	    		System.out.print("Inserisci una directory valida i cui contenuti saranno inviati al Server: ");
	    		inputDirName = reader.readLine();
	    		
	    	}while(!Files.isDirectory(Paths.get(inputDirName)));
    	}catch(IOException e) {
    		e.printStackTrace();
    	}
    	
        System.out.println("La directory " + inputDirName + " è stata accettata!");
		
		// Comunicazione
        Socket clientSocket;
        DataOutputStream dataOut;
        DataInputStream dataIn;
        
        // Creo il file che rappresenta la mia Directory 
        File dir = new File (inputDirName);
        // Ottengo tutti i file della Directory (escludendo le Sub-Directory)
        File[] filesInDir = dir.listFiles(new FileFilter() {
            @Override
            public boolean accept(File file) {
                return file.isFile() && !file.isHidden(); // Ottengo solo i file che non siano nascosti
            }
        });
        
        int response, count = -1;;
        try {
        	
        	// Socket con relativo Timeout
        	clientSocket = new Socket(args[0], port);
            clientSocket.setSoTimeout(MAX_TIME);
            
            // Streams
            dataOut = new DataOutputStream(clientSocket.getOutputStream());
            dataIn = new DataInputStream(clientSocket.getInputStream());
            
            // Per ogni file nell'array
            for(File f : filesInDir) {
            	
            	// Scrivo il nome del file
            	dataOut.writeUTF(f.getName());
            	// Scrivo la sua dimensione 
            	dataOut.writeLong(f.length());
            	
            	// Leggo la risposta del Server
                System.out.println("Aspettando la risposta del Server...");
                response = dataIn.readInt();
                
                // Se la richiesta è stata accettata
                if(response >= 0) {
                	
                	// Re-inizializzo il BufferedReader così da inviare il file
                	reader = new BufferedReader(new FileReader(f));

                    System.out.println("Invio del file " + f.getName() + " al Server...");
                    
                    // Leggo fino a EOF (-1) e lo scrivo nella DataOutputStream
                    while ((count = reader.read()) >= 0) {
                        dataOut.write(count);
                    }
                    
                    reader.close();
                	
                }else {
                	System.err.println("Il Server ha rifiutato la richiesta del file " + f.getName() + "!");
                }
            	
            }
            
            // Chiusura della comunicazione
           	clientSocket.close();
           	dataOut.close();
            dataIn.close();
            System.out.println("Chiusura comunicazione!");
            
        }catch(IOException e) {
        	
        }
		
	}
	
	
	
}
