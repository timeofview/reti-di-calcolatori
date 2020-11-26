import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;


public class Client {


    public static void main(String[] args) {

            int registryRemotePort = 1099;
            String registryRemoteName = "RegistryRemoto";
            String serviceName = "RemOp";

            // Reader input utente 
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

            if (args.length != 1 && args.length != 2) {
                System.err.println("Usage: hostNameRemoteRegistry RmiRegistryPort[OPTIONAL]");
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

            // Recupero il riferimento al servizio remoto presso il RegistryRemoto
            try {
            
                String completeRemoteRegistryName = "//" + registryRemoteHost + ":" + registryRemotePort + "/" + registryRemoteName;
                RemoteRegistryClient remoteRegistry = (RemoteRegistryClient) Naming.lookup(completeRemoteRegistryName);

                String op;
                try {
                    // Trovo il Server attraverso il metodo "search" nel Registry Remoto
                    RemOp server = (RemOp) remoteRegistry.search(serviceName);
                    
                    try {
                        System.out.print("Scrivi il nome dell'operazione: ");
                        
                        while ((op = reader.readLine()) != null) {
                            
                            // Operazione COUNT
                            if (op.equalsIgnoreCase("count")) {
                            
                                int threshold;
                                String filename;
                                
                                System.out.print("\nNome del file: ");
                                filename = reader.readLine();
                                System.out.print("\nLunghezza: ");
                                
                                try {
                                    threshold = Integer.valueOf(reader.readLine());
                                } catch (NumberFormatException e) {
                                    e.printStackTrace();
                                    System.out.print("\nScrivi il nome dell'operazione: ");
                                    continue;
                                }
                                try {
                                    System.out.print("\nNumero di righe che contengono un numero di parole maggiore dell'intero inviato: " + server.countRow(filename, threshold));
                                } catch (RemoteException e) {
                                    e.printStackTrace();
                                }
                                
                            }
                            
                            // Operazione DELETE 
                            else if (op.equalsIgnoreCase("delete")) {
                            
                                int numRow;
                                String filename;
                                
                                System.out.print("\nNome del file: ");
                                filename = reader.readLine();
                                System.out.print("\nRiga da eliminare: ");
                                
                                try {
                                    numRow = Integer.valueOf(reader.readLine());
                                } catch (NumberFormatException e) {
                                    e.printStackTrace();
                                    System.out.print("\nScrivi il nome dell'operazione: ");
                                    continue;
                                }
                                try {
                                    System.out.print("\nNumero di righe presenti nel file modificato: " + server.deleteRow(filename, numRow));
                                } catch (RemoteException e) {
                                    e.printStackTrace();
                                }
                            } else {
                                System.out.println("\nSono permesse solo 2 operazioni:\n1) delete\n2) count");
                            }
                            System.out.print("\nScrivi il nome dell'operazione: ");
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                        System.exit(1);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                    System.exit(2);
                }
            } catch (RemoteException e) {
                e.printStackTrace();
            } catch (MalformedURLException e) {
				e.printStackTrace();
			} catch (NotBoundException e) {
				e.printStackTrace();
			}
    }
}