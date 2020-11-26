import java.rmi.RemoteException;

interface RemoteRegistryTagServer extends RemoteRegistryServer {
    
    // Assegna un tag al nome logico del Server 
    public boolean bindTag(String serverLogicName, String tag) throws RemoteException;
    
}
