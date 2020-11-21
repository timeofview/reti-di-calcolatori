import java.rmi.RemoteException;

interface RemoteRegistryTagClient extends RemoteRegistryClient {
    public String[] searchTag(String tag) throws RemoteException;
}
