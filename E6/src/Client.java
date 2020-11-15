import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class Client {

    public static void main(String[] args) {

        if (args.length != 3) {
            System.out.println("Usage: host port service");
            System.exit(1);
        }

        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String operation;

        try {
            RemOp server = (RemOp) Naming.lookup("//" + args[0] + ":" + args[1] + "/" + args[2]);
            try {
                System.out.println("Scrivi il nome dell'operazione");
                while ((operation = reader.readLine()) != null) {
                    if (operation.equalsIgnoreCase("count")) {
                        int threshold;
                        String filename;
                        System.out.println("Il nome del file: ");
                        filename = reader.readLine();
                        System.out.println("Lunghezza: ");
                        try {
                            threshold = Integer.valueOf(reader.readLine());
                        } catch (NumberFormatException e) {
                            e.printStackTrace();
                            System.out.println("Scrivi il nome dell'operazione");
                            continue;
                        }
                        try {
                            System.out.println("Il numero delle righe che " +
                                    "contengono un numero di parole maggiore dell'intero inviato: "
                                    + server.countRow(filename, threshold));
                        } catch (RemoteException e) {
                            e.printStackTrace();
                        }
                    } else if (operation.equalsIgnoreCase("delete")) {
                        int numRow;
                        String filename;
                        System.out.println("Il nome del file: ");
                        filename = reader.readLine();
                        System.out.println("Riga da eliminare: ");
                        try {
                            numRow = Integer.valueOf(reader.readLine());
                        } catch (NumberFormatException e) {
                            e.printStackTrace();
                            System.out.println("Scrivi il nome dell'operazione");
                            continue;
                        }
                        try {
                            System.out.println("Numero di righe presenti nel file modificato: "
                                    + server.deleteRow(filename, numRow));
                        } catch (RemoteException e) {
                            e.printStackTrace();
                        }
                    } else {
                        System.out.println("Due operazioni sono permesse:delete,count");
                    }
                    System.out.println("Scrivi il nome dell'operazione");
                }
            } catch (IOException e) {
                e.printStackTrace();
                System.exit(5);
            }


        } catch (NotBoundException e) {
            e.printStackTrace();
            System.exit(2);
        } catch (MalformedURLException e) {
            e.printStackTrace();
            System.exit(3);
        } catch (RemoteException e) {
            e.printStackTrace();
            System.exit(4);
        }
    }
}

