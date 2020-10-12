package server;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import com.sun.security.sasl.ServerFactoryImpl;

public class ServerConnectionV2 {
	private ServerController serverController;
	protected ClientList clientList = new ClientList();
	
	public ServerConnectionV2(int port, ServerController serverController) {
		new Connection(port).start();
		this.serverController = serverController;
	}

private class Connection extends Thread {
	private int port;
	private String id = "0";
	
	public Connection(int port) {
		this.port = port;
		
	}


	public void run() {
		Socket clientSocket = null;
		String clientKey = null;
		try (ServerSocket serverSocket = new ServerSocket(port)) {
			System.out.println("Server Socket started: " +  port);
			while(true) {
				try {
					clientSocket = serverSocket.accept();

					System.out.println("Client connected: " + clientSocket.getInetAddress());
					BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
					PrintWriter out = new PrintWriter(new OutputStreamWriter(clientSocket.getOutputStream()));
					System.out.println("Creating new thread for this client: " + clientSocket.getInetAddress());
					
					clientKey = clientSocket.getInetAddress().getHostAddress();
					ClientHandler ch = new ClientHandler(clientSocket,in,out);
					if(clientList.getList().containsKey(clientKey)) {
						System.out.println("Already in list");
					}else {
						id = Integer.toString(Integer.valueOf(id) + 1);
						clientList.addClient(clientKey, ch);
						serverController.ui.addClientToUI(id + ": " + clientKey);
						System.out.println(clientList.getID(clientKey));
						System.out.println(clientKey);
					}
			
					//clientList.addClient(id, ch);
					//String clientID = (id + clientSocket.getInetAddress());
					//serverController.ui.addClientToUI(clientID);
					//System.out.println(clientList.getID(id));
					//System.out.println(clientList.getList());
	
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
					serverController.ui.writeLog(msg);
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

