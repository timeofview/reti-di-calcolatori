import java.rmi.Remote;
import java.rmi.RemoteException;


interface RemoteRegistryClient extends Remote {
    
    // Restituisce il primo server che corrisponde a "logicName"
    public Remote search(String logicName) throws RemoteException;
    
    // Restituisce un array dei Server che corrispondono a "logicName"
    public Remote[] searchAll(String logicName) throws RemoteException;
    
}
