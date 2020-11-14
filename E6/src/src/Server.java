import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.Naming;

// Estende l'implementazione della mia Interfaccia Remota
public class Server extends OpImpl {
	
	
	public Server() {
		
	} 
	
    public static void main(String[] args) { // Registrazione del servizio

		final int DEFAULT_PORT = 1099;
		
    	int port;
    	if(args.length != 1) {
    		port = DEFAULT_PORT;
    	}else{
    		port = Integer.parseInt(args[0]);
    	}
    	
        String registryHost = "localhost";
        String serviceName = "OpInterface";
        
        try {
        	String completeName = "//" + registryHost + ":" + DEFAULT_PORT + "/" + serviceName;
			
        	OpImpl serverRMI = new OpImpl();
			
			// Mette come nome al serverRMI completeName
        	Naming.rebind(completeName, serverRMI);
			
        }catch(Exception e) {
        	e.printStackTrace();
        }
		
		
		System.out.println("Server started!");
		
		
    }
}
