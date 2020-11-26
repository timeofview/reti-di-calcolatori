import java.rmi.RemoteException;

interface RemoteRegistryTagClient extends RemoteRegistryClient {
    
    // Restituisce un array che contiene i nomi logici dei server che corrispondono a "tag"
    public String[] searchTag(String tag) throws RemoteException;
    
}
