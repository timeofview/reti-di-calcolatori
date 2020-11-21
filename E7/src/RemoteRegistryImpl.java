import java.rmi.Naming;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RemoteRegistryImpl extends UnicastRemoteObject {
    final int tableLength = 100;
    Object[][] table = new Object[100][2];

    protected RemoteRegistryIml(int port) throws RemoteException {
        super(port);
    }

    public synchronized Remote search(String nomeLogico)
            throws RemoteException {
        Remote risultato = null;
        if (nomeLogico == null) return null;
        for (int i = 0; i < tableLength; i++)
            if (nomeLogico.equals((String) table[i][0])) {
                risultato = (Remote) table[i][1];
                break;
            }
        return risultato;
    }

    public synchronized Remote[] searchAll(String
                                                    nomeLogico) throws RemoteException {
        int cont = 0;
        if (nomeLogico == null) return new Remote[0];
        for (int i = 0; i < tableLength; i++)
            if (nomeLogico.equals((String) table[i][0]))
                cont++;
        Remote[] risultato = new Remote[cont];
// usato come indice per il riempimento
        cont = 0;
        for (int i = 0; i < tableLength; i++)
            if (nomeLogico.equals((String) table[i][0]))
                risultato[cont++] = (Remote) table[i][1];
        return risultato;
    }

    public synchronized Object[][] getAll()
            throws RemoteException {
        int cont = 0;
        for (int i = 0; i < tableLength; i++)
            if (table[i][0] != null) cont++;

        cont = 0;
        for (int i = 0; i < tableLength; i++)
            if (table[i][0] != null) {
                risultato[cont][0] = table[i][0];
                risultato[cont][1] = table[i][1];
            }
        return risultato;
    }

    public synchronized boolean add(String nomeLogico,
                                         Remote riferimento) throws RemoteException {
        boolean result = false;

        if ((nomeLogico == null) || (riferimento == null))
            return risultato;
        for (int i = 0; i < tableLength; i++)
            if (table[i][0] == null) {
                table[i][0] = nomeLogico;
                table[i][1] = riferimento;
                result = true;
                break;
            }
        return result;
    }

    public synchronized boolean deleteFirst
            (String nomeLogico) throws RemoteException {
        boolean risultato = false;
        if (nomeLogico == null) return risultato;
        for (int i = 0; i < tableLength; i++)
            if (nomeLogico.equals((String) table[i][0])) {
                table[i][0] = null;
                table[i][1] = null;
                risultato = true;
                break;
            }
        return risultato;
    }

    public synchronized boolean deleteAll
            (String nomeLogico) throws RemoteException {
        boolean risultato = false;
        if (nomeLogico == null) return risultato;
        for (int i = 0; i < tableLength; i++)
            if (nomeLogico.equals((String) table[i][0])) {
                if (risultato == false) risultato = true;
                table[i][0] = null;
                table[i][1] = null;
            }
        return risultato;
    }

    public static void main(String[] args) {
        int registryRemotoPort = 1099;
        String registryRemotoHost = "localhost";
        String registryRemotoName = "RegistryRemoto";
        if (args.length != 0 && args.length != 1) // Controllo args
        {
            System.out.println("...");
            System.exit(1);
        }
        if (args.length == 1) {
            try {
                registryRemotoPort = Integer.parseInt(args[0]);
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(1);
        }

        String completeName = "//" + registryRemotoHost + ":" +
                registryRemotoPort + "/" + registryRemotoName;
        try {
            RemoteRegistryIml serverRMI =
                    newRegistryRemotoImpl();
            Naming.rebind(completeName, serverRMI);
        } catch (Exception e) {...}
    }
}
}
