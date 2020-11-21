import java.rmi.RemoteException;

interface RemoteRegistryTagServer extends RemoteRegistryServer {
    public boolean bindTag( String serverLogicName,String tag) throws RemoteException;
}
