import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;
import java.util.HashMap;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;

public class DiscoveryServer {

	public static void main(String[] args) {
		final int BUFF_LENGTH = 256;
		int length = args.length;
		byte[] buff = new byte[BUFF_LENGTH];
		DatagramSocket socket = null;
		DatagramPacket packet = null;
		String path = null;
		// HashMap<String, Integer> map = new HashMap<>();

		if (length % 2 != 1 || length < 3) {
			System.out.println("Sintssi scorretta.");
			System.exit(1);
		}
		int port = Integer.parseInt(args[0]);
		try {
			socket = new DatagramSocket(port);
		} catch (SocketException e) {
			e.printStackTrace();
			System.exit(2);
		}
		int size = length >> 1;
		ArrayList<String> files = new ArrayList<String>();
		ArrayList<Integer> ports = new ArrayList<Integer>();
		int logicLength = 0;
		boolean found = false;

		for (int i = 1; i < length; i+=2) {
			found = false;
			port = Integer.parseInt(args[i+1]);
			for (int j = 0; j < logicLength; j++) {
				if (files.get(j).equals(args[i]) || ports.get(j).equals(port)) {
					found = true;
					break;
				}
			}
			if (!found) {
				files.add(logicLength, args[i]);
				ports.add(logicLength++,port);
				new RowSwapServer(args[i], port).start();
			}

		}
		/*
		 * for (int i = 1; i < length; i += 2) { port = Integer.parseInt(args[i + 1]);
		 * if (!map.containsKey(args[i]) && !map.containsValue(port)) { map.put(args[i],
		 * port); new RowSwapServer(args[i], port).start(); } }
		 */

		packet = new DatagramPacket(buff, BUFF_LENGTH);
		ByteArrayInputStream biStream = null;
		DataInputStream diStream = new DataInputStream(biStream);
		ByteArrayOutputStream boStream = new ByteArrayOutputStream();
		DataOutputStream doStream = new DataOutputStream(boStream);

		byte[] output = null;
		//System.out.println(files[0]);
		//Arrays.sort(files,Collections.reverseOrder());
		//Arrays.sort(files);
		//Arrays.sort(ports);
		//for(int i = 0; i<500; i++){System.out.println(files[i]+" "+ports[i]);}
		//System.out.println(files[50]);
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
			try {
				// doStream.writeInt(map.getOrDefault(diStream.readUTF(), -1));
				//port = -1;
				path = diStream.readUTF();
				/*for (int i = 0; i < logicLength; i++) {
					if (files[i].equals(path)) {
						port = ports[i];
						break;
					}
				}*/	
				//System.out.println(path);
				port=ports.get(files.indexOf(path));
				doStream.writeInt(port);
			} catch (IOException e) {
				e.printStackTrace();
				break;
			}
			output = boStream.toByteArray();
			packet.setData(output, 0, output.length);
			try {
				socket.send(packet);
			} catch (IOException e) {
				e.printStackTrace();
				break;
			}
		}
		socket.close();
	}

}
