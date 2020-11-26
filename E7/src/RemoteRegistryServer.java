import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemoteRegistryServer extends RemoteRegistryClient {
    
    // Restituisce tutti i Server registrati 
    public Object[][] getAll() throws RemoteException;
    
    // I Server si registrano attraverso "add"
    public boolean add(String logicName, Remote reference) throws RemoteException;
    
    // Elimino il primo Server della lista che corrisponde a "logicName"
    public boolean deleteFirst(String logicName) throws RemoteException;
    
    // Elimino tutti i Server che corrispondono a "logicName"
    public boolean deleteAll(String logicName) throws RemoteException;
}

