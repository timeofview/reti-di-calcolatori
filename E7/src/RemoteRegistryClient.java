import java.rmi.Remote;
import java.rmi.RemoteException;


interface RemoteRegistryClient extends Remote {
    public Remote search(String logicName) throws RemoteException;

    public Remote[] searchall(String logicName) throws RemoteException;
}
