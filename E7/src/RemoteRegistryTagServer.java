import java.rmi.RemoteException;

interface RemoteRegistryTagServer extends RemoteRegistryServer {
    public boolean bindTag(String tag, String serverLogicName) throws RemoteException;
}
