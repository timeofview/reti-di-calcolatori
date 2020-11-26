import java.rmi.Naming;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.util.ArrayList;
import java.util.Map;
import java.util.HashMap;
import java.rmi.registry.LocateRegistry;


public class RemoteRegistryImpl extends UnicastRemoteObject implements RemoteRegistryTagClient, RemoteRegistryTagServer {

    private static final long serialVersionUID = 1L;

    final int tableLength = 100;
    Object[][] table = new Object[100][2];
    // Contiene il tag (key) e la relativa lista di nomi logici
    Map<String, ArrayList<String>> tagMap = new HashMap<>(); 
    // Contiene i nomi logici relativi ad un certo tag riferibili
    ArrayList<String> logicNameList = new ArrayList<>();


    protected RemoteRegistryImpl() throws RemoteException {
        super();
        for (int i = 0; i < tableLength; i++) {
            table[i][0] = null;
            table[i][1] = null;
        }
    }
    
    public synchronized Remote search(String logicName) throws RemoteException {

        Remote result = null;

        if (logicName == null) {
            return null;
        }
        for (int i = 0; i < tableLength; i++) {
            if (logicName.equals((String) table[i][0])) {
                result = (Remote) table[i][1];
                
                break;
            }
        }
        return result;
    }

    public synchronized Remote[] searchAll(String logicName) throws RemoteException {

        int count = 0;

        if (logicName == null) {
            return new Remote[0];
        }

        for (int i = 0; i < tableLength; i++) {
            if (logicName.equals((String) table[i][0])) {
                count++;
            }
        }
        
        Remote[] result = new Remote[count];

        count = 0;
        for (int i = 0; i < tableLength; i++) {
            if (logicName.equals((String) table[i][0])) {
                result[count++] = (Remote) table[i][1];
            }
        }
        return result;
    }

    public synchronized Object[][] getAll() throws RemoteException {

        int count = 0;

        for (int i = 0; i < tableLength; i++) {
            if (table[i][0] != null) {
                count++;
            }
        }

        Object[][] result = new Object[count][2];
        count = 0;

        for (int i = 0; i < tableLength; i++) {
            if (table[i][0] != null) {
                result[count][0] = table[i][0];
                result[count][1] = table[i][1];
            }
        }
        
        return result;
    }

    public synchronized boolean add(String logicName, Remote riferimento) throws RemoteException {

        boolean result = false;

        if ((logicName == null) || (riferimento == null)) {
            return result;
        }

        for (int i = 0; i < tableLength; i++) {
            if (table[i][0] == null) {
                table[i][0] = logicName;
                table[i][1] = riferimento;
                
                result = true;
                
                break;
            }
        }

        return result;
    }

    public synchronized boolean deleteFirst(String logicName) throws RemoteException {

        boolean result = false;

        if (logicName == null) {
            return result;
        }

        for (int i = 0; i < tableLength; i++) {
            if (logicName.equals((String) table[i][0])) {
                table[i][0] = null;
                table[i][1] = null;
                
                result = true;

                break;
            }

        }
        return result;
    }

    public synchronized boolean deleteAll(String logicName) throws RemoteException {

        boolean result = false;

        if (logicName == null) {
            return result;
        }
        
        for (int i = 0; i < tableLength; i++) {
            if (logicName.equals((String) table[i][0])) {
            
                if (result == false){
                    result = true;
                }
                
                table[i][0] = null;
                table[i][1] = null;
            }
        }
        
        return result;
    }

    public boolean bindTag(String serverLogicName, String tag) throws RemoteException {

        boolean result = false;

        try {
            // Se la mappa contiene il tag (key) passato al metodo,
            // vado ad aggiungere al campo identificato da quello specifico
            // tag, il nome logico del Server passato come argomento
            if (tagMap.containsKey(tag)) {
                tagMap.get(tag).add(serverLogicName);
                
                result = true;
            } else {
                // Se il tag non esiste nella mappa, vado ad aggiungere
                // una nuova key (ovvero tag) e inizializzo un nuovo campo
                // con all'interno il nome logico del Server passato         
                logicNameList.add(serverLogicName);
                tagMap.put(tag, logicNameList);
            }

        } catch (NullPointerException npe) {
            throw new RemoteException(npe.getMessage());
        }

        return result;

    }

    public String[] searchTag(String tag) throws RemoteException {

        String result[] = new String[tableLength];
        int index = 0;

        if (tagMap.containsKey(tag)) {
            for (String s: tagMap.get(tag)) {
                result[index++] = s;
            }
        }

        return result;
    }

    public static void main(String[] args) {

        int registryRemotePort = 1099;
        String registryRemoteHost = "localhost";
        String registryRemoteName = "RegistryRemoto";

        try {
            LocateRegistry.createRegistry(registryRemotePort);
        } catch (RemoteException e) {
            e.printStackTrace();
            System.exit(1);
        }
        // Controllo args
        if (args.length != 0 && args.length != 1) {

            System.err.println("Usage: Port[OPTIONAL]");
            System.exit(1);
        }

        if (args.length == 1) {
            try {
                registryRemotePort = Integer.parseInt(args[0]);
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
            }
        }

        String completeName = "//" + registryRemoteHost + ":" +
            registryRemotePort + "/" + registryRemoteName;
        try {
            RemoteRegistryImpl serverRMI = new RemoteRegistryImpl();
            Naming.rebind(completeName, serverRMI);
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(2);
        }
        
        System.out.println("RMI inizializzato!");

    }
}