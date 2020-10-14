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
					
					System.out.println(clientIP.substring(10));
					if(clientList.getList().containsKey(clientIP.substring(10))) {
						System.out.println("Already in list");
						ClientHandler ch = new ClientHandler(clientSocket,in,out);
						clientList.addClient(id, ch);
						serverController.ui.addClientToUI(number + ": " + clientIP);

					}else {
						id = Integer.toString(Integer.valueOf(id) + 1);
						number = Integer.toString(Integer.valueOf(number) + 1);
						clientList.addClient(id, null);
						System.out.println(clientList.getID(id));
						System.out.println(clientIP);
						out.write(id);
						out.flush();
					}
	
				}catch (IOException e) {
					System.out.println("Nï¿½got gick fel serversocket");
					if(clientSocket !=null) {
						clientSocket.close();
					}
				}
			}
		} catch (IOException e) {
			System.out.println("Nï¿½got gick fel till kopplingen till servern");
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
	public void sendCharToEveryNode(String scrambledWord) {
		System.out.println("I skicka grejen");
		System.out.println(scrambledWord);
		int counter = 0;
		for (int i = 2; i<clientList.listSize()+2; i++) {
			ClientHandler ch = clientList.getID(Integer.toString(i));
			char c = scrambledWord.charAt(counter);
			System.out.println(c);
			ch.out.write(Character.toString(c));
			ch.out.flush();
			counter++;
		}
	}
}

