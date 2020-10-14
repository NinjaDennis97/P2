package server;

public class ServerTest{
	
	public static void main(String[] args) {
		new ServerTest();
	}
	
	public ServerTest() {
		new ServerController(new UI());
		
	}
}

