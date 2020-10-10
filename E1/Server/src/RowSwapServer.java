import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.BindException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

public class RowSwapServer extends Thread {
	final private int BUFF_LENGTH = 256;
	private String path;
	private int port;
	private DatagramSocket socket = null;
	private DatagramPacket packet = null;
	private int result = -1;
	private byte[] buff = new byte[BUFF_LENGTH];
	private ByteArrayInputStream biStream = null;
	private DataInputStream diStream= null;
	private int row1,row2=0;
	private ByteArrayOutputStream boStream= null;
	private DataOutputStream doStream = null;

	public RowSwapServer(String path, int port) {
		this.path = path;
		this.port = port;
	}

	public void run() {
		try {
			socket = new DatagramSocket(port);
			packet = new DatagramPacket(buff, buff.length);
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
				biStream = new ByteArrayInputStream(packet.getData(),0,packet.getLength());
				diStream = new DataInputStream(biStream);
				row1 = diStream.read();
				row2 = diStream.read();
			}catch (IOException e) {
				e.printStackTrace();
				result =-1;
			}
			/*SWAP*/
			boStream  = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
			try {
				doStream.write(result);
			} catch (IOException e) {
				e.printStackTrace();
			}
			buff = boStream.toByteArray();
			packet.setData(buff);
			try {
				socket.send(packet);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
