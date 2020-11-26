import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;


public class Server extends UnicastRemoteObject implements RemOp {

	private static final long serialVersionUID = 1L;


    protected Server() throws RemoteException {
        super();
    }
    
    // Consente di trovare il numero di righe che contengono un numero di parole maggiore di "threshold"
    @Override
    public int countRow(String fileName, int threshold) throws RemoteException {
    
        FileReader fr = null;
        
        try {
            fr = new FileReader(fileName);
        } catch (FileNotFoundException e) {
            throw new RemoteException("File not found!");
        }
        
        BufferedReader br = new BufferedReader(fr);
        int result = 0;
        String line;
        
        try {
            while ((line = br.readLine()) != null) {

                // Se il numero di parole e' maggiore della soglia passata dal'utente
                if (line.split("\\s+").length > threshold) {
                    result++;
                }
            }
            br.close();
        } catch (IOException e) {
            throw new RemoteException("Errore durante la lettura!");
        }
        return result;
    }

    // Permette di eliminare una riga del file
    @Override
    public synchronized int deleteRow(String fileName, int numRow) throws RemoteException {

        int rows = 0;
        BufferedReader reader;
        PrintWriter writer;
        File originalFile;
        File temp;
        String line;

        try {
            originalFile = new File(fileName);
            reader = new BufferedReader(new FileReader(originalFile));
            temp = new File(fileName + "_tmp.txt");
            writer = new PrintWriter(new FileWriter(temp));
            
            while ((line = reader.readLine()) != null) {
                rows++;
                if (rows != numRow) {
                    writer.println(line);
                }
            }
            
            reader.close();
            writer.close();
            originalFile.delete();
            temp.renameTo(originalFile);
            
            if (rows < numRow)
                throw new RemoteException("Non e' stato possibile eliminare riga " + numRow + ", numero di righe presenti nel file: " + rows);
        } catch (IOException e) {
            throw new RemoteException("Errore durante la lettura!");
        }
        return rows - 1;
    }

    public static void main(String[] args) {
		
		final int SERVER_PORT = 54321;
		
        // Porta di default
        int registryRemotePort = 1099;
        String registryRemoteName = "RegistryRemoto";
        String serviceName = "RemOp";

        // Controllo argomenti
        if (args.length != 1 && args.length != 2) {
            System.err.println("Usage: HostNameRemoteRegistry RmiRegistryPort[OPTIONAL]");
            System.exit(1);
        }
        
        String registryRemoteHost = args[0];

        if (args.length == 2) {
            try {
                registryRemotePort = Integer.parseInt(args[1]);
            } catch (Exception e) {
                System.err.println("Cannot parse Port to integer!");
                System.exit(2);
            }
        }

        // Registrazione servizio presso RegistryRemoto
        String completeRemoteRegistryName = "//" + registryRemoteHost + ":" + registryRemotePort + "/" + registryRemoteName;

        try {
			// Ottengo RMI
            RemoteRegistryTagServer registryRemote = (RemoteRegistryTagServer) Naming.lookup(completeRemoteRegistryName);
			
			// Divento un RMI
            Server serverRMI = new Server();
			
			// Aggiungo il mio servizio con il nome "serviceName"
            registryRemote.add(serviceName, serverRMI);
            registryRemote.bindTag(serviceName, "opFile");
			
			try {
				LocateRegistry.createRegistry(Integer.valueOf(SERVER_PORT));
				Naming.rebind("//" + "localhost" + ":" + SERVER_PORT + "/" + serviceName, serverRMI);
				
			} catch (Exception e) {
				e.printStackTrace();
				System.exit(2);
			}
			
			
        } catch (Exception e) {
            System.err.println("Cannot connect to RMI Registry!");
            e.printStackTrace();
            System.exit(3);
        }
        
        System.out.println("Server inizializzato!");
        
    }
}