package server;

import java.io.BufferedReader;
import java.time.*;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Map;


public class ServerConnectionV2 {
	protected ServerController serverController;
	public ClientList clientList = new ClientList();

	public ServerConnectionV2(int port, ServerController serverController) {
		new Connection(port).start();
		this.serverController = serverController;
	}




	private class Connection extends Thread {
		private int port;
		private String id = "1";
		private String number ="0";
		Socket clientSocket = null;
		String clientIP = null;
		long firstConnection = 0;
		long secondConnection = 0;
		boolean startCounting = false;


		public Connection(int port) {
			this.port = port;
		}

		public void run() {
			/*
			 * klient connecta, ge klienten en ip 2-9 skicka tillbaka sista delen i ip addressen. 
			 * */


			try (ServerSocket serverSocket = new ServerSocket(port)) {
				System.out.println("Server Socket started: " +  port);
				while(true) {
					try {
						clientSocket = serverSocket.accept();

						System.out.println("Client connected: " + clientSocket.getInetAddress());
						BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
						PrintWriter out = new PrintWriter(new OutputStreamWriter(clientSocket.getOutputStream()));
						System.out.println("Creating new thread for this client: " + clientSocket.getInetAddress());
						//Lägg till att varje gång ny anslut så plusa connectedNodes.
						clientIP = clientSocket.getInetAddress().getHostAddress();

						if (startCounting = true) {
							secondConnection = System.currentTimeMillis();
						}

						System.out.println(clientIP.substring(10));
						if(clientList.getList().containsKey(clientIP.substring(10))) {
							System.out.println("Already in list");
							ClientHandler ch = new ClientHandler(clientSocket,in,out, id);
							clientList.addClient(id, ch);
							serverController.ui.addClientToUI(number + ": " + clientIP);
							firstConnection = 0;
							secondConnection = 0;
							startCounting = false;

						}else {
							id = Integer.toString(Integer.valueOf(id) + 1);
							number = Integer.toString(Integer.valueOf(number) + 1);
							clientList.addClient(id, null);
							System.out.println(clientList.getID(id));
							System.out.println(clientIP);
							out.write(id);
							out.flush();
							firstConnection = System.currentTimeMillis();
							startCounting = true;
						}

						if ((secondConnection - firstConnection) > 10000) {
							clientList.remove(id);
							System.out.println("Tog bort från listan.");
							id = Integer.toString(Integer.valueOf(id) - 1);
							firstConnection = 0;
							secondConnection = 0;
							startCounting = false;
						}

					}catch (IOException e) {
						System.out.println("Något gick fel serversocket");
						if(clientSocket !=null) {
							clientSocket.close();
						}
					}
				}
			} catch (IOException e) {
				System.out.println("Något gick fel till kopplingen till servern");
			}
		}


	}
	public void sendCharToEveryNode(String scrambledWord) {
		System.out.println("I skicka grejen");
		System.out.println(scrambledWord);
		String ordet = serverController.word1;
		String ordet2 = serverController.word2;
		int counter = 0;
		//	for (int i = 2; i<clientList.listSize()+2; i++) {
		//		ClientHandler ch = clientList.getID(Integer.toString(i));
		//		if(ch != null) {
		//		char c = scrambledWord.charAt(counter);
		//		System.out.println(c);
		//		ch.out.write(Character.toString(c));
		//		ch.out.flush();
		//		counter++;
		//	}
		//	}
		for(Map.Entry<String, ClientHandler> entry: clientList.clientList.entrySet()) {
			String ID = entry.getKey();
			ClientHandler ch = entry.getValue();
			if (ch != null) {
				char c = scrambledWord.charAt(counter);
				System.out.println(c);
				//ch.out.write(Character.toString(c));
				ch.out.write(ordet + ordet2);
				ch.out.flush();
				counter++;
			}
		}
	}

	public class ClientHandler extends Thread {
		final Socket clientSocket;
		final BufferedReader in;
		final PrintWriter out;
		final String id;
		private String korrekt = "korrekt";
		private String fel = "fel";
		public ClientHandler(Socket clientSocket, BufferedReader in, PrintWriter out, String id ) {
			System.out.println("In ClientHandler!!");
			this.clientSocket = clientSocket;
			this.in = in;
			this.out = out;
			this.id = id;
			start();
		}

		public void run(){
			while(true) {
				try {
					String msg = in.readLine();
					serverController.ui.writeLog(msg);
					System.out.println(msg);
					System.out.println(serverController.word1 + serverController.word2);
					if(msg.length() > 1) {
						if(msg.toLowerCase().equals(serverController.word1)) {
							out.write(korrekt);
							out.flush();
						}
						else if(msg.toLowerCase().equals(serverController.word2)) {
							out.write(korrekt);
							out.flush();
						}
						else if(msg.toLowerCase().equals(serverController.word1 +  serverController.word2)) {
							out.write(korrekt);
							out.flush();
						}
						else if(msg.toLowerCase().equals(serverController.word2 +  serverController.word1)) {
							out.write(korrekt);
							out.flush();
						} else {
							out.write(fel);
							out.flush();
						}
					}

				} catch (IOException e) {
					try {
						System.out.println("Closing Streams!");
						in.close();
						out.close();
						clientSocket.close();
						clientList.remove(id);
						System.out.println("Tog bort " + id);
					} catch (IOException e1) {
					}
					return;
				}
			}
		}
	}
}

