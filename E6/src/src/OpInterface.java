import java.rmi.Remote;
import java.rmi.RemoteException;

// Nome vecchio: RemOp.java

// Creazione interfaccia remota per la nostra applicazione
public interface OpInterface extends Remote {
    
    // Definizione dei metodi del nostro oggetto remoto
    // Attenzione: Ogni metodo dovrà avere "throws RemoteException"! 
    public int countRow(String filename, int threshold) throws RemoteException;

    public int deleteRow(String filename, int numRow) throws RemoteException;
}
