import java.rmi.RemoteException;

interface RemoteRegistryTagClient extends RemoteRegistyClient {
    public String[] searchTag(String tag) throws RemoteException;
}