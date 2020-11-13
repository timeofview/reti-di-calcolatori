import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class ServerImpl extends UnicastRemoteObject implements RemOp {


    protected ServerImpl() throws RemoteException {
    }

    @Override
    public int countRow(String filename, int threshold) throws RemoteException {
        return 0;
    }

    @Override
    public int deleteRow(String filename, int numRow) throws RemoteException {
        return 0;
    }

    public static void main(String[] args) {
        final int DEFAULT_PORT = 54321;


    }
}
