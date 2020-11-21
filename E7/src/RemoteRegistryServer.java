import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemoteRegistryServer
        extends RemoteRegistryClient {

    public Object[][] getAll()
            throws RemoteException;

    public boolean add(String logicName,
                       Remote reference) throws RemoteException;

    public boolean deleteFirst(String logicName)
            throws RemoteException;

    public boolean deleteAll(String logicName)
            throws RemoteException;
}
