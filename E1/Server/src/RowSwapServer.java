import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class RowSwapServer extends Thread {
	final private int BUFF_LENGTH = 256;
	private String path;
	private int port;
	private DatagramSocket socket = null;
	private DatagramPacket packet = null;
	private int result = -1;
	private byte[] buff = new byte[BUFF_LENGTH];
	private byte[] output = null;
	private ByteArrayInputStream biStream = null;
	private DataInputStream diStream = null;
	private int row1, row2 = 0;
	private ByteArrayOutputStream boStream = null;
	private DataOutputStream doStream = null;

	public RowSwapServer(String path, int port) {
		this.path = path;
		this.port = port;
	}

	public void run() {
		try {
			socket = new DatagramSocket(port);
			packet = new DatagramPacket(buff, buff.length);
			boStream = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
		} catch (SocketException e) {
			e.printStackTrace();
			System.exit(2);
		}
		while (true) {
			result = 1;
			try {
				packet.setData(buff);
				socket.receive(packet);
			} catch (IOException e) {
				e.printStackTrace();
				continue;
			}
			try {
				biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
				diStream = new DataInputStream(biStream);
				row1 = diStream.readInt();
				row2 = diStream.readInt();
			} catch (IOException e) {
				e.printStackTrace();
				result = -1;
			}
			int result = -1;
			int currentRow = 0;
			String c;
			String rowOne = null;
			String rowTwo = null;
			BufferedWriter out;
			File textOut = null;
			try {
				BufferedReader buff = new BufferedReader(new FileReader(path));

				while ((c = buff.readLine()) != null) {
					currentRow++;
					if (currentRow == row1)
						rowOne = c;
					else if (currentRow == row2)
						rowTwo = c;
				}
				System.out.print(rowOne + "-" + row1 + "    " + rowTwo + "-" + row2);
				buff.close();

				buff = new BufferedReader(new FileReader(path));
				textOut = new File(path + "_temp");

				out = new BufferedWriter(new FileWriter(textOut));
				c = buff.readLine();
				currentRow = 0;
				while (c != null) {
					currentRow++;
					if (currentRow == row1) {
						out.write(rowTwo);
						out.newLine();
					} else if (currentRow == row2) {
						out.write(rowOne);
						out.newLine();
					} else {
						out.write(c);
						out.newLine();
					}
					c = buff.readLine();
				}
				out.flush();
				out.close();
				buff.close();
			} catch (IOException e) {
				System.out.print("File non trovato \n");
			}
			try {
				Files.deleteIfExists(Paths.get(path));
			} catch (IOException e) {
				System.out.println("File non rimosso");
			} 

			textOut.renameTo(new File(path));
			result = 1;

			try {
				doStream.writeInt(result);
			} catch (IOException e) {
				e.printStackTrace();
			}
			output = boStream.toByteArray();
			packet.setData(output, 0, output.length);
			try {
				socket.send(packet);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
}
