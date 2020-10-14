package server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class ServerConnectionV2 {
	private ServerController serverController;
	protected ClientList clientList = new ClientList();
	
	public ServerConnectionV2(int port, ServerController serverController) {
		new Connection(port).start();
		this.serverController = serverController;
	}

private class Connection extends Thread {
	private int port;
	private String id = "1";
	
	public Connection(int port) {
		this.port = port;
		
	}
	
	
	public void run() {
		Socket clientSocket = null;
		try (ServerSocket serverSocket = new ServerSocket(port)) {
			System.out.println("Server Socket started: " +  port);
			while(true) {
				try {
				clientSocket = serverSocket.accept();
				System.out.println("New client connected" + clientSocket.getInetAddress());
				
				BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
				PrintWriter out = new PrintWriter(new OutputStreamWriter(clientSocket.getOutputStream()));
				System.out.println("Skapar ny tr�d f�r denna clienten" + clientSocket.getInetAddress());
				ClientHandler ch = new ClientHandler(clientSocket,in,out);
				
				clientList.addClient(id, ch);
				String clientID = (id + clientSocket.getInetAddress());
				serverController.gui.addClientToUI(clientID);
				System.out.println(clientList.getID(id));
				//Skapar ny tr�d objekt
				//Thread clientThread = new ClientHandler(clientSocket, in, out);
				//clientThread.start();
				
				}catch (IOException e) {
					System.out.println("N�got gick fel serversocket");
					if(clientSocket !=null) {
						clientSocket.close();
					}
				}
			}
		} catch (IOException e) {
			System.out.println("N�got gick fel till kopplingen till servern");
		}
	}
}
	
	public class ClientHandler extends Thread {
		final Socket clientSocket;
		final BufferedReader in;
		final PrintWriter out;
		public ClientHandler(Socket clientSocket, BufferedReader in, PrintWriter out ) {
			System.out.println("In ClientHandler!!");
			this.clientSocket = clientSocket;
			this.in = in;
			this.out = out;
			start();
		}

		public void run(){
			while(true) {
				try {
					String msg = in.readLine();
					System.out.println(msg);
					serverController.gui.writeLog(msg);
					out.write(msg);
					out.flush();
					System.out.println("Server send message!");
				} catch (IOException e) {
					try {
						System.out.println("Closing Streams!");
						in.close();
						out.close();
						clientSocket.close();
					} catch (IOException e1) {
					}
					return;
				}
			}
		}
	}
}

