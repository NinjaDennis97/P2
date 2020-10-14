package server;

import java.util.HashMap;

import server.ServerConnectionV2.ClientHandler;

public class ClientList {
	public HashMap<String , ClientHandler> clientList = new HashMap<String, ClientHandler>();
	
	public synchronized HashMap<String, ClientHandler> getList(){
		return clientList;
	}
	
	public synchronized void addClient(String ID, ClientHandler client) {
		clientList.put(ID, client);
	}
	
	public synchronized ClientHandler getID(String ID) {
		for(String s : clientList.keySet()) {
			if(s.equals(ID)) {
				return clientList.get(s);
			}
		}
		return null;
	}
	
	public synchronized void remove(String ID) {
		clientList.remove(ID);
	}
	
	public synchronized boolean alreadyListed(String ID) {
		for(String s : clientList.keySet()) {
			if(s.equals(ID)) {
				return true;
			}
		}
		return false;
	}
	
	public synchronized int listSize() {
		return clientList.size();
	}
	
}
