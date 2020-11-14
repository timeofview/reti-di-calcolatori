import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemOp extends Remote {
    public int countRow(String filename, int threshold) throws RemoteException;

    public int deleteRow(String filename, int numRow) throws RemoteException;
}
