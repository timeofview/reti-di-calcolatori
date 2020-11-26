import java.rmi.Remote;
import java.rmi.RemoteException;

public interface RemOp extends Remote {
    
    // Consente di trovare il numero di righe che contengono un numero di parole maggiore di "threshold"
    public int countRow(String filename, int threshold) throws RemoteException;
    
    // Permette di eliminare una riga del file
    public int deleteRow(String filename, int numRow) throws RemoteException;
    
}

