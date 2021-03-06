import java.io.*;
import java.rmi.Naming;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.server.UnicastRemoteObject;

public class ServerImpl extends UnicastRemoteObject implements RemOp {

    protected ServerImpl() throws RemoteException {
    }

    @Override
    public int countRow(String fileName, int threshold) throws RemoteException {
        FileReader fr = null;
        try {
            fr = new FileReader(fileName);
        } catch (FileNotFoundException e) {
            throw new RemoteException("File not found!");
        }
        BufferedReader br = new BufferedReader(fr);
        int result = 0;
        String line;
        try {
            while ((line = br.readLine()) != null) {
                // Se il numero di parole e' maggiore della soglia passata dal'utente
                if (line.split("\\s+").length > threshold) {
                    result++;
                }
            }
            br.close();
        } catch (IOException e) {
            throw new RemoteException("Errore durante la lettura");
        }
        return result;
    }

    // Prende in ingresso il nome del file, elimina la riga passata come  restituisce un intero,
    @Override
    public synchronized int deleteRow(String fileName, int numRow) throws RemoteException {
        int rows = 0;
        BufferedReader reader;
        PrintWriter writer;
        File originalFile;
        File temp;
        String line;

        try {
            originalFile = new File(fileName);
            reader = new BufferedReader(new FileReader(originalFile));
            temp = new File(fileName + "_tmp.txt");
            writer = new PrintWriter(new FileWriter(temp));
            while ((line = reader.readLine()) != null) {
                rows++;
                if (rows != numRow) {
                    writer.println(line);
                }
            }
            reader.close();
            writer.close();
            originalFile.delete();
            temp.renameTo(originalFile);
            if (rows < numRow)
                throw new RemoteException("Non e' stato possibile eliminare riga " + numRow
                        + ", numero di righe presenti nel file: " + rows);
        } catch (IOException e) {
            throw new RemoteException("Errore durante la lettura");
        }
        return rows - 1;
    }

    public static void main(String[] args) {
        if (args.length != 2) {
            System.out.println("Usage: port servicename");
            System.exit(1);
        }
        try {
            LocateRegistry.createRegistry(Integer.valueOf(args[0]));
            ServerImpl serverRMI = new ServerImpl();
            Naming.rebind("//" + "localhost" + ":" + args[0] + "/" + args[1], serverRMI);
        } catch (Exception e) {
            e.printStackTrace();
            System.exit(2);
        }
        System.out.println("Server started!");
    }
}
