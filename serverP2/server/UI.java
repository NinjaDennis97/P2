package server;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;

import javax.swing.*;

public class UI extends JFrame {
	
	private JTextArea logText = new JTextArea();
	private JTextArea connectedNodes = new JTextArea();
	private JScrollPane logWindow = new JScrollPane(logText);
	private JScrollPane  nodeWindow = new JScrollPane (connectedNodes);
	private JLabel serverIP = new JLabel();
	private JLabel correctWord = new JLabel();
	private JPanel rightSide = new JPanel();
	private JPanel topPanel = new JPanel();
	private static int port = 3500;
	
	
	
	
	public UI() {
		try {
			InetAddress iA = InetAddress.getLocalHost();
			serverIP.setText("Server IP:" + iA.getHostAddress() );
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		topPanel.setLayout(new GridLayout(1,2));
		correctWord.setText("Targer Word:" + "asdfghjkloiuytre");
		
		topPanel.add(correctWord);
		topPanel.add(serverIP);
		

		setLayout(new GridLayout(1,2));
		rightSide.setLayout(new BorderLayout());
		rightSide.add(nodeWindow, BorderLayout.CENTER);
		rightSide.add(topPanel, BorderLayout.NORTH);
		
		

		logText.setLineWrap(true);
		logText.setWrapStyleWord(true);
		logText.setEditable(false);



		connectedNodes.setLineWrap(true);
		connectedNodes.setWrapStyleWord(true);
		connectedNodes.setEditable(false);


		add(logWindow);
		add(rightSide);
		setMinimumSize(new Dimension(700,200));
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		
		setSize(700,450);
		setVisible(true);
	}
	
	public void writeLog(String txt) {
		logText.append(txt + "\n" );
	}
	
	public void setCorrectWord(String targetWord) {
		correctWord.setText("Target: " + targetWord );
	}
	
	public void addClientToUI(String newClient){
		connectedNodes.append(newClient + "\n");
	}


	/*public static void main(String args[]) throws IOException {
		UI ui = new UI();
		ServerController sc = new ServerController(ui);
		ServerConnectionV2 sc2 = new ServerConnectionV2(port);
	}*/
}