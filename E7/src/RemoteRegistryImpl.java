import java.rmi.Naming;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

public class RemoteRegistryImpl extends UnicastRemoteObject implements RemoteRegistryTagClient, RemoteRegistryTagServer {
    final int tableLength = 100;
    Object[][] table = new Object[100][3];

    protected RemoteRegistryImpl() throws RemoteException {
        super();
    }

    public synchronized Remote search(String logicName)
            throws RemoteException {
        Remote result = null;
        if (logicName == null) return null;
        for (int i = 0; i < tableLength; i++)
            if (logicName.equals((String) table[i][0])) {
                result = (Remote) table[i][1];
                break;
            }
        return result;
    }

    @Override
    public Remote[] searchall(String logicName) throws RemoteException {
        return new Remote[0];
    }

    public synchronized Remote[] searchAll(String
                                                   logicName) throws RemoteException {
        int count = 0;
        if (logicName == null) return new Remote[0];
        for (int i = 0; i < tableLength; i++)
            if (logicName.equals((String) table[i][0]))
                count++;
        Remote[] result = new Remote[count];

        count = 0;
        for (int i = 0; i < tableLength; i++)
            if (logicName.equals((String) table[i][0]))
                result[count++] = (Remote) table[i][1];
        return result;
    }

    public synchronized Object[][] getAll()
            throws RemoteException {
        int count = 0;
        for (int i = 0; i < tableLength; i++)
            if (table[i][0] != null) count++;
        Object[][] result = new Object[count][2];
        count = 0;
        for (int i = 0; i < tableLength; i++)
            if (table[i][0] != null) {
                result[count][0] = table[i][0];
                result[count][1] = table[i][1];
            }
        return result;
    }

    public synchronized boolean add(String logicName,
                                    Remote riferimento) throws RemoteException {
        boolean result = false;

        if ((logicName == null) || (riferimento == null))
            return result;
        for (int i = 0; i < tableLength; i++)
            if (table[i][0] == null) {
                table[i][0] = logicName;
                table[i][1] = riferimento;
                result = true;
                break;
            }
        return result;
    }

    public synchronized boolean deleteFirst
            (String logicName) throws RemoteException {
        boolean result = false;
        if (logicName == null) return result;
        for (int i = 0; i < tableLength; i++)
            if (logicName.equals((String) table[i][0])) {
                table[i][0] = null;
                table[i][1] = null;
                result = true;
                break;
            }
        return result;
    }

    public synchronized boolean deleteAll
            (String logicName) throws RemoteException {
        boolean result = false;
        if (logicName == null) return result;
        for (int i = 0; i < tableLength; i++)
            if (logicName.equals((String) table[i][0])) {
                if (result == false) result = true;
                table[i][0] = null;
                table[i][1] = null;
            }
        return result;
    }

    @Override
    public String[] searchTag(String tag) throws RemoteException {
        String result[] = new String[tableLength];
        int index = 0;
        for (int i = 0; i < tableLength; i++) {
            if (tag.equals(table[i][2])) {
                result[index++] = (String) table[i][0];//uccidetemi, sto castando object a string;
            }
        }
        return result;
    }

    @Override
    public boolean bindTag(String serverLogicName, String tag) throws RemoteException {
        boolean result = false;
        try {
            for (int i = 0; i < tableLength; i++) {
                if (serverLogicName.equals(table[i][0])) {
                    table[i][2] = tag;
                    result = true;
                }
            }
        } catch (NullPointerException npe) {
            throw new RemoteException(npe.getMessage());
        }
        return result;
    }

    public static void main(String[] args) {
        int registryRemotoPort = 1099;
        String registryRemotoHost = "localhost";
        String registryRemotoName = "RegistryRemoto";
        if (args.length != 0 && args.length != 1) // countrollo args
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
                RemoteRegistryImpl serverRMI =
                        new RemoteRegistryImpl();
                Naming.rebind(completeName, serverRMI);
            } catch (Exception e) {
                e.printStackTrace();
                System.exit(2);
            }
        }
    }


}
