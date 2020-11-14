import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.Serializable;
import java.rmi.RemoteException;


// Classe di implementazione del metodo dell'interfaccia remota
public class OpImpl implements Serializable, OpInterface {
    
    // In un file testo, contare le righe che contengono un numero di
    // parole superiore ad un intero espresso dal cliente
    public int countRow(String fileName, int threshold) throws RemoteException {

        FileReader fr = null;
        try {
            fr = new FileReader(fileName);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        BufferedReader br = new BufferedReader(fr);

        
        int result = 0;
        String[] parts;
        String line;
        try {
            while ((line = br.readLine()) != null) {
            
                // Splitto la riga letta dal file
                parts = line.split("\\s+");

                // Se il numero di parole è maggiore della soglia passata dal'utente
                if(parts.length > threshold){
                    result++;
                }
                
            }
            // Chiusura BufferedReader
            br.close();
            
        } catch (IOException e) {
            e.printStackTrace();
        }
            
        return result;

    }
    
    
    // Prende in ingresso il nome del file, elimina la riga passata come  restituisce un intero, 
    
    //@Override
    public int deleteRow(String fileName, int numRow) throws RemoteException {
        	
    	int rows = 0;
        BufferedReader reader; 
	File originalFile = new File(fileName);
    	try {
            reader = new BufferedReader(new FileReader(originalFile));
            
            while(reader.readLine()!=null && rows < numRow) {
                rows++;
            }
            
            reader.close();
            
            if(rows < numRow)
                throw new RemoteException();
				
		} catch(IOException e) {
			e.printStackTrace();
		}
		
	File temp;
	PrintWriter writer;
	String line;
	try {
			
			reader = new BufferedReader(new FileReader(originalFile));
			temp = new File(fileName + "_tmp.txt");
			writer = new PrintWriter(new FileWriter(temp));
			
			rows=1;
            // Conto le righe per vedere se è necessario creare un file d'appoggio
			while((line=reader.readLine())!=null) { 
				if(rows != numRow) {
					writer.println(reader);
				}
				rows++;
			}
			
			writer.close();
			reader.close();
            		
			originalFile.delete();
			temp.renameTo(originalFile);
            
			
		} catch (IOException e) {
		
			e.printStackTrace();
		}
        
		return rows-2;
        
    }
    
    
}
