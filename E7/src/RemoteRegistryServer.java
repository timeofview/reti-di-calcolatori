import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemoteRegistryServer
        extends RemoteRegistyClient {

    public Object[][] getAll()
            throws RemoteException;

    public boolean add(String logicName,
                       Remote reference) throws RemoteException;

    public boolean deleteFirst(String nomeLogico)
            throws RemoteException;

    public boolean deleteAll(String nomeLogico)
            throws RemoteException;
}
