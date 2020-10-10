import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.HashMap;

public class DiscoveryServer {

	public static void main(String[] args) {
		final int BUFF_LENGTH = 256;
		int length = args.length;
		byte[] buff = new byte[BUFF_LENGTH];
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		HashMap<String, Integer> map = new HashMap<>();
		if (length % 2 != 1 || length < 3) {
			System.out.println("Sintssi scortetta.");
			System.exit(1);
		}
		int port = Integer.parseInt(args[0]);
		try {
			socket = new DatagramSocket(port);
		} catch (SocketException e) {
			e.printStackTrace();
			System.exit(2);
		}
		for (int i = 1; i < length; i += 2) {
			port = Integer.parseInt(args[i + 1]);
			if (!map.containsKey(args[i]) && !map.containsValue(port)) {
				map.put(args[i], port);
				new RowSwapServer(args[i], port).start();
			}
		}

		packet = new DatagramPacket(buff, BUFF_LENGTH);
		ByteArrayInputStream biStream = null;
		DataInputStream diStream = null;
		ByteArrayOutputStream boStream = null;
		DataOutputStream doStream = null;
		map.entrySet().stream().forEach(System.out::println);
		while (true) {
			packet.setData(buff);
			try {
				socket.receive(packet);
			} catch (IOException e) {
				e.printStackTrace();
				continue;
			}
			biStream = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
			diStream = new DataInputStream(biStream);
			boStream = new ByteArrayOutputStream();
			doStream = new DataOutputStream(boStream);
			try {
				doStream.writeInt(map.getOrDefault(diStream.readUTF(), -1));
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				break;
			}
			buff = boStream.toByteArray();
			packet.setData(buff, 0, buff.length);
			try {
				socket.send(packet);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				break;
			}
		}
		socket.close();

	}

}
